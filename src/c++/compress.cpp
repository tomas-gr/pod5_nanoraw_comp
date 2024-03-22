#include <boost/filesystem.hpp>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <stdexcept>

#include "compressor.h"

#include "pod5_format/c_api.h"
#include "pod5_management_utils.h"

typedef struct 
{
	std::string in_filename, out_filename;
    std::string bam_filename;
    std::string levels_table_filename;
} compress_cli_args;

compress_cli_args parse_args(int argc, char **argv, char *curr_dir)
{
	compress_cli_args res;

	if (argc > 5)
		std::cerr << "Ignoring extra arguments (only first 4 considered)\n";
	
    if (argc <= 1)
		std::cerr << "No input file specified\n";
	if (argc <= 2)
		std::cerr << "No output file specified\n";
    if (argc <= 3)
        std::cerr << "No BAM file specified\n";
    if (argc <= 4)
        std::cerr << "No levels table specified\n";

    if (argc < 5)
    {
        exit(EXIT_FAILURE);
    }

	res.in_filename = argv[1];
	res.out_filename = argv[2];
	if (is_path_relative(res.in_filename))
		res.in_filename = std::string(curr_dir) + '/' + res.in_filename;
	if (is_path_relative(res.out_filename))
		res.out_filename = std::string(curr_dir) + '/' + res.out_filename;

    res.bam_filename = argv[3];
    res.levels_table_filename = argv[4];
	return res;
}

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
	const Pod5WriterOptions_t writer_options = {0, UNCOMPRESSED_SIGNAL, 0, 0};

	pod5_init();

	reader = pod5_open_file(args.in_filename.data());
	EXIT_PROGRAM_ON_FAIL(reader, nullptr)

	if (boost::filesystem::exists(boost::filesystem::path(args.out_filename.data())))
	{
		remove(args.out_filename.data());
	}

	writer = pod5_create_file(args.out_filename.data(), "PGNano compressor", &writer_options);
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

    pgnano::standalone::Compressor compressor(args.bam_filename, args.levels_table_filename);

	for (size_t current_batch_idx = 0; current_batch_idx < batch_count; current_batch_idx++)
	{
		Pod5ReadRecordBatch_t *current_batch;
		LOG_PROGRAM_ERROR(pod5_get_read_batch(&current_batch, reader, current_batch_idx))
		size_t batch_row_count;
		LOG_PROGRAM_ERROR(pod5_get_read_batch_row_count(&batch_row_count, current_batch))
		ReadBatchRowInfo_t read_record_batch_array[batch_row_count]; // For large batches this could cause a stack overflow. Should push data to heap
		size_t sample_count[batch_row_count];
		int16_t *original_signal[batch_row_count];
        uint32_t original_signal_length[batch_row_count];
        int16_t *compressed_signal[batch_row_count];
        uint32_t compressed_signal_words[batch_row_count];//FIXME: returned value should be uint32_t and not a size_t converted to uint32_t

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

			original_signal[current_batch_row] =
				(int16_t *)malloc(sizeof(int16_t) * sample_count[current_batch_row]);
			LOG_PROGRAM_ERROR(pod5_get_read_complete_signal(
				reader, current_batch, current_batch_row,
				sample_count[current_batch_row], original_signal[current_batch_row]))
			original_signal_length[current_batch_row] = sample_count[current_batch_row];
            size_t compressed_signal_max_length = compressor.compressed_signal_max_size(original_signal_length[current_batch_row]);
            compressed_signal[current_batch_row] = (int16_t *) malloc(compressed_signal_max_length);//FIXME:Incorrect memory alloc
            compressed_signal_words[current_batch_row] = compressor.compress(from_pod5_id(read_record_batch_array[current_batch_row].read_id),original_signal[current_batch_row], original_signal_length[current_batch_row], compressed_signal[current_batch_row]);
            free(original_signal[current_batch_row]); // TODO: can optimize
		}

		static ReadBatchRowInfoArray_t flattened_array;
		transform_read_data_batch_array(read_record_batch_array, batch_row_count, current_batch, writer, &flattened_array);
		LOG_PROGRAM_ERROR(pod5_add_reads_data(// FIXME: incorrect signal array passed
			writer, batch_row_count, READ_BATCH_ROW_INFO_VERSION, &flattened_array,
			const_cast<const int16_t **>(compressed_signal), compressed_signal_words))

		free_batch_array(&flattened_array);

		LOG_PROGRAM_ERROR(pod5_free_read_batch(current_batch))
	}
	uint64_t samples_processed, bytes_written;
	samples_processed = compressor.samples_processed();
	bytes_written = compressor.signal_16_bit_words_written() * 2;
	std::cout << bytes_written << ";" << samples_processed;

	free(curr_dir);
	release_pod5_resources(reader, writer);
	return EXIT_SUCCESS;
}
