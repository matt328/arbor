using Glade.Logging;
using Microsoft.UI;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Media;
using System;

namespace Glade.Converters
{
  internal partial class LogLevelToBrush : IValueConverter
  {
    public object Convert(object value, Type targetType, object parameter, string language)
    {
      if (value is not LogLevel level)
        return new SolidColorBrush(Colors.Black);

      return level switch
      {
        LogLevel.Info => new SolidColorBrush(Colors.LightGray),
        LogLevel.Warning => new SolidColorBrush(Colors.Goldenrod),
        LogLevel.Error => new SolidColorBrush(Colors.IndianRed),
        LogLevel.Debug => new SolidColorBrush(Colors.LightBlue),
        _ => new SolidColorBrush(Colors.Black),
      };
    }

    public object ConvertBack(object value, Type targetType, object parameter, string language)
        => throw new NotImplementedException();
  }
}
