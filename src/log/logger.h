
#pragma once

#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <source_location>
#include <sstream>

namespace aime::log {

// Define your own severity enumeration.
enum class LogLevel { Trace, Debug, Info, Warning, Error, Fatal };

// The LoggerEntry helper accumulates parts of the log message (using
// operator<<) and, when destroyed, sends the accumulated message to Boost.Log
// along with the current local time and the provided caller file/line.
class LoggerEntry {
public:
  LoggerEntry(LogLevel lvl, const std::source_location &loc)
      : level(lvl), location(loc) {}

  template <typename T> LoggerEntry &operator<<(const T &value) {
    oss << value;
    return *this;
  }

  ~LoggerEntry() {
    std::ostringstream prefix;
    // Retrieve current local time
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    prefix << std::put_time(std::localtime(&now_time), "%F %T") << " ";
    // Use the caller's source location (from the marker macro)
    prefix << location.file_name() << ":" << location.line() << " ";
    switch (level) {
    case LogLevel::Trace:
      BOOST_LOG_TRIVIAL(trace) << prefix.str() << " TRACE; " << oss.str();
      break;
    case LogLevel::Debug:
      BOOST_LOG_TRIVIAL(debug) << prefix.str() << " DEBUG; " << oss.str();
      break;
    case LogLevel::Info:
      BOOST_LOG_TRIVIAL(info) << prefix.str() << " INFO; " << oss.str();
      break;
    case LogLevel::Warning:
      BOOST_LOG_TRIVIAL(warning) << prefix.str() << " WARNING; " << oss.str();
      break;
    case LogLevel::Error:
      BOOST_LOG_TRIVIAL(error) << prefix.str() << " ERROR; " << oss.str();
      break;
    case LogLevel::Fatal:
      BOOST_LOG_TRIVIAL(fatal) << prefix.str() << " FATAL; " << oss.str();
      break;
    }
  }

private:
  LogLevel level;
  std::source_location location;
  std::ostringstream oss;
};

// A simple helper structure that holds our log level and source location.
struct LogMarker {
  LogLevel level;
  std::source_location loc;
};

// The Logger class initializes Boost.Log.
class Logger {
public:
  Logger() {
    // Initial Boost.Log setup.
    boost::log::add_file_log(boost::log::keywords::file_name =
                                 "/home/ananda/logs/server.log",
                             boost::log::keywords::auto_flush = true);
    boost::log::add_console_log(std::cout);
    boost::log::add_common_attributes();
  }

  // Overload operator<< to begin a log entry using a LogMarker.
  // This will capture the source location provided by the caller's macro.
  LoggerEntry operator<<(const LogMarker &marker) const {
    return LoggerEntry(marker.level, marker.loc);
  }
};

// Define the LOG macro that creates a LogMarker with the provided LogLevel
// and captures std::source_location::current() at the call site.
#define LOG(level)                                                             \
  (::aime::log::LogMarker{level, std::source_location::current()})

// Convenient log level constants for succinct syntax.
constexpr LogLevel trace = LogLevel::Trace;
constexpr LogLevel debug = LogLevel::Debug;
constexpr LogLevel info = LogLevel::Info;
constexpr LogLevel warning = LogLevel::Warning;
constexpr LogLevel error = LogLevel::Error;
constexpr LogLevel fatal = LogLevel::Fatal;

} // namespace aime::log