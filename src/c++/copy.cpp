#define COMPRESSOR_N03

#include <boost/filesystem.hpp>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <stdexcept>

#include "pod5_format/c_api.h"
#include "pod5_management_utils.h"

typedef struct 
{
	std::string in_filename, out_filename;
	CompressionOption comp_opt;
} copy_cli_args;

copy_cli_args parse_args(int argc, char **argv, char *curr_dir)
{
	copy_cli_args res;

	if (argc > 4)
	{
		std::cerr << "Ignoring extra arguments (only first 3 considered)\n";
	}
	if (argc == 1)
	{
		std::cerr << "No input file specified\n";
		exit(EXIT_FAILURE);
	}
	if (argc == 2)
	{
		std::cerr << "No output file specified\n";
		exit(EXIT_FAILURE);
	}
	res.comp_opt = DEFAULT_SIGNAL_COMPRESSION;
	if (strncmp(argv[3], "--VBZ", 5) == 0)
	{
		res.comp_opt = VBZ_SIGNAL_COMPRESSION;
	}
	else if (strncmp(argv[3], "--uncompressed", 14) == 0)
	{
		res.comp_opt = UNCOMPRESSED_SIGNAL;
	}
	else if (strncmp(argv[3], "--pgnano", 8) == 0)
	{
		res.comp_opt = PGNANO_SIGNAL_COMPRESSION;
	}
	else
	{
		std::cerr << "Incorrect compression method";
		exit(EXIT_FAILURE);
	}
	
	res.in_filename = argv[1];
	res.out_filename = argv[2];
	if (is_path_relative(res.in_filename))
		res.in_filename = std::string(curr_dir) + '/' + res.in_filename;
	if (is_path_relative(res.out_filename))
		res.out_filename = std::string(curr_dir) + '/' + res.out_filename;
	return res;
}

long full_size_keys = 0;
long full_size_S = 0;
long full_size_M = 0;
long full_size_Llow = 0;
long full_size_Lhigh = 0;
long full_size_data = 0;

long comp_size_keys = 0;
long comp_size_S = 0;
long comp_size_M = 0;
long comp_size_Llow = 0;
long comp_size_Lhigh = 0;
long comp_size_data = 0;

long total_samples = 0;

double compression_time = 0;
double decompression_time = 0;

long number_small = 0;
long number_medium = 0;
long number_large = 0;

int main(int argc, char **argv)
{

	char *curr_dir = alloc_and_get_curr_dir_name();

	pore_type_cache = std::make_unique<std::map<std::string, int16_t>>();
	if (pore_type_cache == nullptr)
	{
		EXIT_PROGRAM_ON_FAIL(nullptr, nullptr)
	}

	run_info_id_map = std::make_unique<std::map<int16_t, int16_t>>();
	if (run_info_id_map == nullptr)
	{
		EXIT_PROGRAM_ON_FAIL(nullptr, nullptr)
	}
	

	auto args = parse_args(argc, argv, curr_dir);//FIXME: memory leak on curr_dir if parse fails

	Pod5FileReader_t *reader;
	Pod5FileWriter_t *writer;
	const Pod5WriterOptions_t writer_options = {0, args.comp_opt, 0, 0};

	pod5_init();

	reader = pod5_open_file(args.in_filename.data());
	EXIT_PROGRAM_ON_FAIL(reader, nullptr)

	if (boost::filesystem::exists(boost::filesystem::path(args.out_filename.data())))
	{
		remove(args.out_filename.data());
	}

	writer = pod5_create_file(args.out_filename.data(), "Python API", &writer_options);
	EXIT_PROGRAM_ON_FAIL(reader, writer)
	size_t batch_count;
	LOG_PROGRAM_ERROR(pod5_get_read_batch_count(&batch_count, reader))

	run_info_index_t file_run_info_count;
	LOG_PROGRAM_ERROR(pod5_get_file_run_info_count(reader, &file_run_info_count))
	for (run_info_index_t i = 0; i < file_run_info_count; i++)
	{
		RunInfoDictData_t *run_info_struct;
		pod5_get_file_run_info(reader, i, &run_info_struct);
		LOG_PROGRAM_ERROR(pod5_add_run_info_wrapped(i, writer, run_info_struct))
		pod5_free_run_info(run_info_struct);
	}

	for (size_t current_batch_idx = 0; current_batch_idx < batch_count; current_batch_idx++)
	{
		Pod5ReadRecordBatch_t *current_batch;
		LOG_PROGRAM_ERROR(pod5_get_read_batch(&current_batch, reader, current_batch_idx))
		size_t batch_row_count;
		LOG_PROGRAM_ERROR(pod5_get_read_batch_row_count(&batch_row_count, current_batch))
		ReadBatchRowInfo_t read_record_batch_array[batch_row_count]; // For large batches this could cause a stack overflow. Should push data to heap
		size_t sample_count[batch_row_count];
		int16_t *signal[batch_row_count];

		for (size_t current_batch_row = 0; current_batch_row < batch_row_count;
			 current_batch_row++)
		{
			// Load ReadBatchRowInfo to memory
			uint16_t read_table_version;
			LOG_PROGRAM_ERROR(pod5_get_read_batch_row_info_data(
				current_batch, current_batch_row, READ_BATCH_ROW_INFO_VERSION,
				&read_record_batch_array[current_batch_row], &read_table_version));

			LOG_PROGRAM_ERROR(pod5_get_read_complete_sample_count(
				reader, current_batch, current_batch_row,
				&sample_count[current_batch_row]))

			signal[current_batch_row] = // FIXME: POD5 already alloc's memory...
				(int16_t *)malloc(sizeof(int16_t) * sample_count[current_batch_row]);
			LOG_PROGRAM_ERROR(pod5_get_read_complete_signal(
				reader, current_batch, current_batch_row,
				sample_count[current_batch_row], signal[current_batch_row]))
		}

		uint32_t signal_length[batch_row_count];
		for (size_t i = 0; i < batch_row_count; i++)
		{
			signal_length[i] = sample_count[i];
		}

		static ReadBatchRowInfoArray_t flattened_array;
		transform_read_data_batch_array(read_record_batch_array, batch_row_count, current_batch, writer, &flattened_array);
		LOG_PROGRAM_ERROR(pod5_add_reads_data(
			writer, batch_row_count, READ_BATCH_ROW_INFO_VERSION, &flattened_array,
			const_cast<const int16_t **>(signal), signal_length))

		free_batch_array(&flattened_array);

		for (size_t i = 0; i < batch_row_count; i++)
		{
			free(signal[i]);
		}
		LOG_PROGRAM_ERROR(pod5_free_read_batch(current_batch))
	}

	uint_fast64_t bytes_written, total_sample_count;
	pgnano_get_compression_stats(&bytes_written, &total_sample_count);
	// std::cout << bytes_written << ";" << total_sample_count << std::endl;

	/*
	For each compressor there are differente buffers being used:
	
	keys -> stores the keys for decoding the data buffers
	data -> stores the full values indiscriminately (used only by KD)
	S -> stores the values that can be written in a half byte (4 bits)
	M -> Stores the values that can be written in a byte (8 bits) (For l_h it also stores the values that would go to Llow)
	Llow -> Stores the low byte of numbers that are bigger than 273 (8 bits)
	Lhigh -> Stores the high byte of numbers that are bigger than 273 (8 bits)
	*/

	#ifdef COMPRESSOR_VBZ
		long comp_total_size = comp_size_keys + comp_size_S + comp_size_M + comp_size_Llow + comp_size_Lhigh + comp_size_data;
		long full_total_size = full_size_keys + full_size_S + full_size_M + full_size_Llow + full_size_Lhigh + full_size_data;

		printf("Samples: %ld\n", total_samples);
		printf("Bytes after coding: %ld\n", full_total_size);
		printf("Bytes after compression: %ld\n", comp_total_size);
		printf("bps compression: %f\n", (double)comp_total_size * 8 / (double)total_samples);
		printf("\n");
		printf("Compression Time (s): %f\n", compression_time);
		printf("Decompression Time (s): %f\n", decompression_time);
		printf("\n");
	#endif
	#ifdef COMPRESSOR_VBZ1
		long comp_total_size = comp_size_keys + comp_size_S + comp_size_M + comp_size_Llow + comp_size_Lhigh + comp_size_data;
		long full_total_size = full_size_keys + full_size_S + full_size_M + full_size_Llow + full_size_Lhigh + full_size_data;

		printf("Samples: %ld\n", total_samples);
		printf("Bytes after coding: %ld\n", full_total_size);
		printf("Bytes after compression: %ld\n", comp_total_size);
		printf("bps compression: %f\n", (double)comp_total_size * 8 / (double)total_samples);
		printf("\n");
		printf("keys coded size: %ld\n", full_size_keys);
		printf("data coded size: %ld\n", full_size_M);
		printf("\n");
		printf("Compression Time (s): %f\n", compression_time);
		printf("Decompression Time (s): %f\n", decompression_time);
		printf("\n");
	#endif
    #ifdef COMPRESSOR_KD
		long comp_total_size = comp_size_keys + comp_size_S + comp_size_M + comp_size_Llow + comp_size_Lhigh + comp_size_data;
		long full_total_size = full_size_keys + full_size_S + full_size_M + full_size_Llow + full_size_Lhigh + full_size_data;

		printf("Samples: %ld\n", total_samples);
		printf("Bytes after coding: %ld\n", full_total_size);
		printf("Bytes after compression: %ld\n", comp_total_size);
		printf("bps compression: %f\n", (double)comp_total_size * 8 / (double)total_samples);
		printf("\n");
		printf("keys coded size: %ld\n", full_size_keys);
		printf("data coded size: %ld\n", full_size_data);
		printf("\n");
		printf("keys comp size: %ld\n", comp_size_keys);
		printf("data comp size: %ld\n", comp_size_data);
		printf("\n");
		printf("Keys bps: %f\n", (double)comp_size_keys * 8 / (double)full_size_keys);
		printf("data bps: %f\n", (double)comp_size_data * 8 / (double)full_size_data);	
		printf("\n");
		printf("Compression Time (s): %f\n", compression_time);
		printf("Decompression Time (s): %f\n", decompression_time);
		printf("\n");
    #endif
	#ifdef COMPRESSOR_LH
		long comp_total_size = comp_size_keys + comp_size_S + comp_size_M + comp_size_Llow + comp_size_Lhigh + comp_size_data;
		long full_total_size = full_size_keys + full_size_S + full_size_M + full_size_Llow + full_size_Lhigh + full_size_data;

		printf("Samples: %ld\n", total_samples);
		printf("Bytes after coding: %ld\n", full_total_size);
		printf("Bytes after compression: %ld\n", comp_total_size);
		printf("bps compression: %f\n", (double)comp_total_size * 8 / (double)total_samples);
		printf("\n");
		printf("keys coded size: %ld\n", full_size_keys);
		printf("M coded size: %ld\n", full_size_M);
		printf("Lhigh coded size: %ld\n", full_size_Lhigh);
		printf("\n");
		printf("keys comp size: %ld\n", comp_size_keys);
		printf("M comp size: %ld\n", comp_size_M);
		printf("Lhigh comp size: %ld\n", comp_size_Lhigh);
		printf("\n");	
		printf("Keys bps: %f\n", (double)comp_size_keys * 8 / (double)full_size_keys);
		printf("M bps: %f\n", (double)comp_size_M * 8 / (double)full_size_M);
		printf("Lhigh bps: %f\n", (double)comp_size_Lhigh * 8 / (double)full_size_Lhigh);
		printf("\n");
		printf("Compression Time (s): %f\n", compression_time);
		printf("Decompression Time (s): %f\n", decompression_time);
		printf("\n");
	#endif
    #ifdef COMPRESSOR_LL_LH
		long comp_total_size = comp_size_keys + comp_size_S + comp_size_M + comp_size_Llow + comp_size_Lhigh + comp_size_data;
		long full_total_size = full_size_keys + full_size_S + full_size_M + full_size_Llow + full_size_Lhigh + full_size_data;

		printf("Samples: %ld\n", total_samples);
		printf("Bytes after coding: %ld\n", full_total_size);
		printf("Bytes after compression: %ld\n", comp_total_size);
		printf("bps compression: %f\n", (double)comp_total_size * 8 / (double)total_samples);
		printf("\n");
		printf("keys coded size: %ld\n", full_size_keys);
		printf("M coded size: %ld\n", full_size_M);
		printf("Llow coded size: %ld\n", full_size_Llow);
		printf("Lhigh coded size: %ld\n", full_size_Lhigh);
		printf("\n");
		printf("keys comp size: %ld\n", comp_size_keys);
		printf("M comp size: %ld\n", comp_size_M);
		printf("Llow comp size: %ld\n", comp_size_Llow);
		printf("Lhigh comp size: %ld\n", comp_size_Lhigh);
		printf("\n");
        printf("Keys bps: %f\n", (double)comp_size_keys * 8 / (double)full_size_keys);
		printf("M bps: %f\n", (double)comp_size_M * 8 / (double)full_size_M);
		printf("Llow bps: %f\n", (double)comp_size_Llow * 8 / (double)full_size_Llow);
		printf("Lhigh bps: %f\n", (double)comp_size_Lhigh * 8 / (double)full_size_Lhigh);
		printf("\n");
		printf("Compression Time (s): %f\n", compression_time);
		printf("Decompression Time (s): %f\n", decompression_time);
		printf("\n");
    #endif
    #ifdef COMPRESSOR_N01
		long comp_total_size = comp_size_keys + comp_size_S + comp_size_M + comp_size_Llow + comp_size_Lhigh + comp_size_data;
		long full_total_size = full_size_keys + full_size_S + full_size_M + full_size_Llow + full_size_Lhigh + full_size_data;

		printf("Samples: %ld\n", total_samples);
		printf("Bytes after coding: %ld\n", full_total_size);
		printf("Bytes after compression: %ld\n", comp_total_size);
		printf("bps compression: %f\n", (double)comp_total_size * 8 / (double)total_samples);
		printf("\n");
		printf("keys coded size: %ld\n", full_size_keys);
		printf("S coded size: %ld\n", full_size_S);
		printf("M coded size: %ld\n", full_size_M);
		printf("Llow coded size: %ld\n", full_size_Llow);
		printf("Lhigh coded size: %ld\n", full_size_Lhigh);
		printf("\n");
		printf("keys comp size: %ld\n", comp_size_keys);
		printf("S comp size: %ld\n", comp_size_S);
		printf("M comp size: %ld\n", comp_size_M);
		printf("Llow comp size: %ld\n", comp_size_Llow);
		printf("Lhigh comp size: %ld\n", comp_size_Lhigh);
		printf("\n");
		printf("Keys bps: %f\n", (double)comp_size_keys * 8 / (double)full_size_keys);
		printf("S bps: %f\n", (double)comp_size_S * 8 / (double)full_size_S);
		printf("M bps: %f\n", (double)comp_size_M * 8 / (double)full_size_M);
		printf("Llow bps: %f\n", (double)comp_size_Llow * 8 / (double)full_size_Llow);
		printf("Lhigh bps: %f\n", (double)comp_size_Lhigh * 8 / (double)full_size_Lhigh);
		printf("\n");
		printf("Compression Time (s): %f\n", compression_time);
		printf("Decompression Time (s): %f\n", decompression_time);
		printf("\n");
    #endif
    #ifdef COMPRESSOR_N02
		long comp_total_size = comp_size_keys + comp_size_S + comp_size_M + comp_size_Llow + comp_size_Lhigh + comp_size_data;
		long full_total_size = full_size_keys + full_size_S + full_size_M + full_size_Llow + full_size_Lhigh + full_size_data;

		printf("Samples: %ld\n", total_samples);
		printf("Bytes after coding: %ld\n", full_total_size);
		printf("Bytes after compression: %ld\n", comp_total_size);
		printf("bps compression: %f\n", (double)comp_total_size * 8 / (double)total_samples);
		printf("\n");
		printf("keys coded size: %ld\n", full_size_keys);
		printf("S coded size: %ld\n", full_size_S);
		printf("M coded size: %ld\n", full_size_M);
		printf("Llow coded size: %ld\n", full_size_Llow);
		printf("Lhigh coded size: %ld\n", full_size_Lhigh);
		printf("\n");
		printf("keys comp size: %ld\n", comp_size_keys);
		printf("S comp size: %ld\n", comp_size_S);
		printf("M comp size: %ld\n", comp_size_M);
		printf("Llow comp size: %ld\n", comp_size_Llow);
		printf("Lhigh comp size: %ld\n", comp_size_Lhigh);
		printf("\n");
		printf("Keys bps: %f\n", (double)comp_size_keys * 8 / (double)full_size_keys);
		printf("S bps: %f\n", (double)comp_size_S * 8 / (double)full_size_S);
		printf("M bps: %f\n", (double)comp_size_M * 8 / (double)full_size_M);
		printf("Llow bps: %f\n", (double)comp_size_Llow * 8 / (double)full_size_Llow);
		printf("Lhigh bps: %f\n", (double)comp_size_Lhigh * 8 / (double)full_size_Lhigh);
		printf("\n");
		printf("Compression Time (s): %f\n", compression_time);
		printf("Decompression Time (s): %f\n", decompression_time);
		printf("\n");
    #endif
    #ifdef COMPRESSOR_N03
		long comp_total_size = comp_size_keys + comp_size_S + comp_size_M + comp_size_Llow + comp_size_Lhigh + comp_size_data;
		long full_total_size = full_size_keys + full_size_S + full_size_M + full_size_Llow + full_size_Lhigh + full_size_data;

		printf("Samples: %ld\n", total_samples);
		printf("Bytes after coding: %ld\n", full_total_size);
		printf("Bytes after compression: %ld\n", comp_total_size);
		printf("bps compression: %f\n", (double)comp_total_size * 8 / (double)total_samples);
		printf("\n");
		printf("keys coded size: %ld\n", full_size_keys);
		printf("S coded size: %ld\n", full_size_S);
		printf("M coded size: %ld\n", full_size_M);
		printf("Llow coded size: %ld\n", full_size_Llow);
		printf("Lhigh coded size: %ld\n", full_size_Lhigh);
		printf("\n");
		printf("keys comp size: %ld\n", comp_size_keys);
		printf("S comp size: %ld\n", comp_size_S);
		printf("M comp size: %ld\n", comp_size_M);
		printf("Llow comp size: %ld\n", comp_size_Llow);
		printf("Lhigh comp size: %ld\n", comp_size_Lhigh);
		printf("\n");
		printf("Keys bps: %f\n", (double)comp_size_keys * 8 / (double)full_size_keys);
		printf("S bps: %f\n", (double)comp_size_S * 8 / (double)full_size_S);
		printf("M bps: %f\n", (double)comp_size_M * 8 / (double)full_size_M);
		printf("Llow bps: %f\n", (double)comp_size_Llow * 8 / (double)full_size_Llow);
		printf("Lhigh bps: %f\n", (double)comp_size_Lhigh * 8 / (double)full_size_Lhigh);
		printf("\n");
		printf("Compression Time (s): %f\n", compression_time);
		printf("Decompression Time (s): %f\n", decompression_time);
		printf("\n");
	#endif

	free(curr_dir);
	release_pod5_resources(reader, writer);
	return EXIT_SUCCESS;
}
