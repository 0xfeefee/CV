
// Dependencies (3rd party):
#include <chrono>
#include <ratio>

namespace cv {

	/*
		Check if all the fixed-size integer aliases are of correct size.
	*/
	static_assert(sizeof(s8)  == 1);
	static_assert(sizeof(s16) == 2);
	static_assert(sizeof(s32) == 4);
	static_assert(sizeof(s64) == 8);
	static_assert(sizeof(u8)  == 1);
	static_assert(sizeof(u16) == 2);
	static_assert(sizeof(u32) == 4);
	static_assert(sizeof(u64) == 8);
	static_assert(sizeof(f32) == 4);
	static_assert(sizeof(f64) == 8);

	// @temporary: 64 bit arch only:
	static_assert(sizeof(u8*) == 8);
}

/*
## Logger implementation
*/
#ifdef PROJECT_ENABLE_LOGS
	// Dependencies (3rd party):
	#include <spdlog/spdlog.h>
	#include <spdlog/sinks/basic_file_sink.h>
	#include <spdlog/sinks/stdout_color_sinks.h>
	#include <spdlog/sinks/rotating_file_sink.h>

	namespace cv {

		/*
			Generates the log output file name for the current day formatted in ISO 8601 format: { YYYY-MM-DD },
			e.g. "2024-08-29".
		*/
		static inline const std::string
		get_log_filename(const std::string& name) {
			// Get the current system time epoch:
		    std::time_t epoch_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

		    // Convert it to local time:
		    std::tm local_time;

		    #ifdef PROJECT_PLATFORM_WIN64
			    errno_t errors = localtime_s(&local_time, &epoch_time);
			#else
			    #error "Unix path for { get_log_filename } not implemented!"
			#endif

		    ERROR_IF(errors != 0, "Failed convert Epoch time to local time!");

		    // Return the current date as a string:
		    return fmt::format(
		    	"logs/{:04}-{:02}-{:02}_{}.log",
		    	local_time.tm_year + 1900,
		    	local_time.tm_mon,
		    	local_time.tm_mday,
		    	name
		    );
		}

		/*
			In the constructor we initialize all the spdlog loggers. We have 2 loggers:
			- First for info messages, which are of low importance and will only be stored in the { *_info.log }
			  files.
			- Second for warning and error messages, which are of high importance and will be sent to both the
			  standard out (terminal) and { *_warn.log } files. Warnings and errors will also be printed out in
			  color.

			Files are organized per day, and every day can have up to 8 files per logger, when one file reaches
			the size of 4MB new file is created or we cycle back to the first file and overwrite if we already
			reached the 8 file limit.

			Both loggers are asynchronous and have the following format:

			.cmd
				[2024-08-29, 23:01:22][I]
				This is an info message.

				[2024-08-29, 23:01:22][W]
				This is a warning message.

				[2024-08-29, 23:01:22][E]
				This is an error message.
		*/
		Log_Context::Log_Context() {
			const std::string info_file = get_log_filename("info");
			const std::string warn_file = get_log_filename("warn");

			spdlog::init_thread_pool(8192, 1);

			/*
				Create the log sinks, file sink is rotating in order to prevent having one giant incomprehensible
				file with all the logs in it, at the moment we settle for up to 8 files and each up to 4MB of size.
			*/
			constexpr int FILE_CAPACITY = 1024 * 1024 * 4;

			auto info_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(info_file, FILE_CAPACITY, 8);
			auto warn_file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(warn_file, FILE_CAPACITY, 8);
			auto console_sink 	= std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

			// Create asynchronous loggers:
			info_logger = std::make_shared<spdlog::async_logger>(
				"async_info_logger",
				spdlog::sinks_init_list{info_file_sink},
				spdlog::thread_pool(),
				spdlog::async_overflow_policy::block
			);
			info_logger->set_pattern("\n[%Y-%m-%d, %T][%L]\n%v");

			warn_logger = std::make_shared<spdlog::async_logger>(
				"async_warn_logger",
				spdlog::sinks_init_list{warn_file_sink, console_sink},
				spdlog::thread_pool(),
				spdlog::async_overflow_policy::block
			);
			warn_logger->set_pattern("\n[%Y-%m-%d, %T][%L]\n%^%v%$");

			// Set sink levels:
			info_file_sink->set_level(spdlog::level::info);
			warn_file_sink->set_level(spdlog::level::warn);
			console_sink->set_level(spdlog::level::warn);

			// Set logger levels:
			info_logger->set_level(spdlog::level::info);
			warn_logger->set_level(spdlog::level::warn);
		}

		Log_Context::~Log_Context() {
			spdlog::shutdown();
		}

		/*
			Log should look like this:

			[YYYY-MM-DD, HH:MM:SS][E]
			* /foo/bar/baz/file.cxx (487)
			* Optional message, which may include data, id= 42
		*/
		void
		terminate(cstr_t file_name, int line, cstr_t condition_str, cstr_t message) {
			Shared<spdlog::async_logger> logger = get_instance<Log_Context>()->warn_logger;

			std::string format = (message == nullptr ? "* {} ({})\n* {}" : "* {} ({})\n* {}\n* {}");
			logger->error(format, file_name, line, condition_str, message);

			// Terminate the application.
			exit(1);
		}
	}
#endif // PROJECT_ENABLE_LOGS
