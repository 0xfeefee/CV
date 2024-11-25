
#pragma once
/*
# Base PCH

	Precompiled headers are automatically included in every file which makes them a good place to define
	utilities and language features used. It's not perfect, but it declutters a lot of files.

	Since we are doing more modern flavor of C++ we have to churn through a lot of templated code which may
	slow the build down. For this project we tolerate builds under 3 seconds (when compiled with MSVC), after
	that we introduce unity builds ...
*/

/*
## Dependencies & aliases
*/
#include <string>
#include <memory>
#include <vector>
#include <deque>
#include <set>
#include <unordered_map>
#include <typeindex>


template <typename T>
using Unique = std::unique_ptr<T>;

template <typename T>
using Shared = std::shared_ptr<T>;


/*
## Utility macros

	Defined here because implementation of certain functions depend on them.

	- { EXPECT } is an assert which is a bit nicer to use.
	- { ERORR_IF } is essentially EXPECT negated, exists for semantic reasons.
	- { WARN_IF } raises a warning when the condition is fulfilled.
*/
#if PROJECT_ENABLE_LOGS
	#define EXPECT(condition, ...)   if (!(condition)) cv::terminate(__FILE__, __LINE__, #condition, __VA_ARGS__)
	#define ERROR_IF(condition, ...) if ((condition))  cv::terminate(__FILE__, __LINE__, #condition, __VA_ARGS__)
#else
	#define EXPECT(condition, ...)
	#define ERROR_IF(condition, ...)
#endif


namespace cv {

	/*
	## Fixed-size integer types

		These types should be used when the size of the variable is important, otherwise just use an { int }.
		This is the same as what { stdint.h } would define.
	*/
	typedef signed   char        s8;  // int8_t
	typedef signed   short       s16; // int16_t
	typedef signed   int         s32; // int32_t
	typedef signed 	 long long   s64; // int64_t
	typedef unsigned char        u8;  // uint8_t
	typedef unsigned short       u16; // uint16_t
	typedef unsigned int         u32; // uint32_t
	typedef unsigned long long   u64; // uint64_t, size_t

	constexpr s8  S8_MIN  = (-127i8 - 1);
	constexpr s16 S16_MIN = (-32767i16 - 1);
	constexpr s32 S32_MIN = (-2147483647i32 - 1);
	constexpr s64 S64_MIN = (-9223372036854775807i64 - 1);
	constexpr s8  S8_MAX  = 127i8;
	constexpr s16 S16_MAX = 32767i16;
	constexpr s32 S32_MAX = 2147483647i32;
	constexpr s64 S64_MAX = 9223372036854775807i64;
	constexpr u8  U8_MAX  = 0xffui8;
	constexpr u16 U16_MAX = 0xffffui16;
	constexpr u32 U32_MAX = 0xffffffffui32;
	constexpr u64 U64_MAX = 0xffffffffffffffffui64;

	/*
	## More aliases
	*/
	typedef float  		 f32;
	typedef double 		 f64;
	typedef const char*  cstr_t;

	/*
		Terminate the program with a message of where it happened, used by { EXPECT, ERROR_IF } macro defined above.
	*/
	void
	terminate(cstr_t file_name, int line, cstr_t condition_str, cstr_t message = nullptr);

	/*
		A lazy helper function useful for creating "context", this instance is shared between threads.
	*/
	template <typename T>
	Unique<T>& get_context() {
		static Unique<T> instance = std::make_unique<T>();
		ERROR_IF(!instance);

		return instance;
	}

	/*
		This is same as { get_context } but this instance is not shared between threads.
	*/
	template <typename T>
	Unique<T>& get_local_context() {
		static thread_local Unique<T> instance = std::make_unique<T>();
		ERROR_IF(!instance);

		return instance;
	}

	/*
		Read the given file into a string.
	*/
	std::string
	read_entire_file(cstr_t file_name);
}


/*
## Logger

	Including these here because at the moment I'm prioritizing ease of use over compilation speed.
	Don't use spdlog directly, use { log, log_warn, log_error } instead.

	Logger will output in { /logs } directory relative to the executable, there are 2 types of logs { info, warn }.
	General information ends up in { info } logs, warnings and errors end up in { warn } logs.

	To enable/disable the logger set the { PROJECT_ENABLE_LOGS } CMake option.
*/
#if PROJECT_ENABLE_LOGS
	#include <utility> 		  // forward
	#include <spdlog/async.h> // async_logger

	namespace cv {
		/*
			Log_Context is used for internal logger state, it's only here because we have decided to prioritize
			the ease of use of { log, log_warn, log_error } functions. But there's no need to ever use this
			directly!
		*/
		struct Log_Context {
			Shared<spdlog::async_logger> info_logger;
			Shared<spdlog::async_logger> warn_logger; // Also used for errors

			Log_Context();
			~Log_Context();
		};

		/*
			This logs a regular message, that's just supposed to be a note or something equally unimportant.
		*/
		template <typename ...Args>
		void
		log(cstr_t fmt, Args&&... args) {
			get_context<Log_Context>()->info_logger->info(fmt, std::forward<Args>(args)...);
		}

		/*
			This logs out a message of mild importance, this message may be part of the reason why an error
			happened and is therefore logged in the same file as errors.
		*/
		template <typename ...Args>
		void
		log_warn(cstr_t fmt, Args&&... args) {
			get_context<Log_Context>()->warn_logger->warn(fmt, std::forward<Args>(args)...);
		}

		/*
			This logs out an error message, but it will not terminate the program like { ERROR_IF }.
		*/
		template <typename ...Args>
		void
		log_error(cstr_t fmt, Args&&... args) {
			get_context<Log_Context>()->warn_logger->error(fmt, std::forward<Args>(args)...);
		}
	}
#else // PROJECT_ENABLE_LOGS=0
	/*
		Logger functions expand to no-op when logger is disabled.
	*/
	namespace cv {

		template <typename ...Args>
		inline void
		log(cstr_t fmt, Args&&... args) {}

		template <typename ...Args>
		inline void
		log_warn(cstr_t fmt, Args&&... args) {}

		template <typename ...Args>
		inline void
		log_error(cstr_t fmt, Args&&... args) {}

	}

#endif // PROJECT_ENABLE_LOGS
