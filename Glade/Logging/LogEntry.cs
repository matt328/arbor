using System;

namespace Glade.Logging
{

  internal enum LogLevel
  {
    Trace, Debug, Info, Warning, Error, Critical,
  }

  internal class LogEntry
  {
    public DateTime TimeStamp { get; set; } = DateTime.Now;
    public LogLevel Level { get; set; }
    public string Message { get; set; } = string.Empty;
  }
}
