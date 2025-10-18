using CommunityToolkit.Mvvm.ComponentModel;
using System.Collections.ObjectModel;

namespace Glade.Logging
{
  internal partial class LogStore : ObservableObject
  {
    public ObservableCollection<LogEntry> Entries { get; set; } = [];

    public void Add(LogLevel level, string message)
    {
      Entries.Add(new LogEntry { Level = level, Message = message });
    }
  }
}
