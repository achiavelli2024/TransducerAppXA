using System;
using System.IO;
using System.Text;
using System.Diagnostics;

namespace Transducer_Estudo
{
    // Logger simples e thread-safe usado pelo projeto.
    // Mantém assinatura mínima: Configure, Log, LogFmt, LogHex, LogException, FilePath.
    // Comentários explicam cada método.
    public static class TransducerLogger
    {
        private static readonly object _locker = new object();
        private static bool _enabled = true;
        private static string _folder = null;
        public static string FilePath { get; private set; }

        // Configure: define pasta de logs e habilita/desabilita
        public static void Configure(string folderOrFilePath, bool enabled = true)
        {
            try
            {
                _enabled = enabled;
                if (string.IsNullOrWhiteSpace(folderOrFilePath))
                {
                    folderOrFilePath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "transducerapp", "logs");
                }

                // se o parâmetro terminar com ".log" ou conter extensão, assume arquivo; caso contrário assume pasta
                if (Path.HasExtension(folderOrFilePath))
                {
                    FilePath = folderOrFilePath;
                    _folder = Path.GetDirectoryName(folderOrFilePath);
                }
                else
                {
                    _folder = folderOrFilePath;
                    FilePath = Path.Combine(_folder, "transducer_debug.log");
                }

                if (!Directory.Exists(_folder))
                    Directory.CreateDirectory(_folder);

                // cria arquivo inicial
                lock (_locker)
                {
                    File.AppendAllText(FilePath, $"{DateTime.Now:yyyy-MM-dd HH:mm:ss.fff} - TransducerLogger configured{Environment.NewLine}");
                }
            }
            catch (Exception ex)
            {
                // não lança — apenas escreve em Debug
                Debug.Print("TransducerLogger.Configure failed: " + ex.Message);
            }
        }

        // Log básico (uma linha)
        public static void Log(string message)
        {
            if (!_enabled) return;
            try
            {
                string line = $"{DateTime.Now:yyyy-MM-dd HH:mm:ss.fff} - {message}";
                Debug.Print(line);
                lock (_locker)
                {
                    File.AppendAllText(FilePath, line + Environment.NewLine, Encoding.UTF8);
                }
            }
            catch { }
        }

        // Log formatado (string.Format)
        public static void LogFmt(string format, params object[] args)
        {
            try
            {
                Log(string.Format(format, args));
            }
            catch (Exception ex)
            {
                Log("LogFmt format error: " + ex.Message);
            }
        }

        // Log de exceção com stacktrace
        public static void LogException(Exception ex, string context = null)
        {
            if (ex == null) return;
            try
            {
                var msg = new StringBuilder();
                msg.AppendFormat("{0} - EXCEPTION: {1}", DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff"), context ?? "Exception");
                msg.AppendLine();
                msg.AppendLine(ex.ToString());
                string line = msg.ToString();
                Debug.Print(line);
                lock (_locker)
                {
                    File.AppendAllText(FilePath, line + Environment.NewLine, Encoding.UTF8);
                }
            }
            catch { }
        }

        // Log de buffer em hexa (útil para RX/TX bruto)
        public static void LogHex(string title, byte[] buffer, int offset = 0, int count = 0)
        {
            try
            {
                if (buffer == null) return;
                if (count == 0) count = buffer.Length - offset;
                if (count <= 0) return;

                var sb = new StringBuilder();
                sb.AppendFormat("{0} - {1} bytes - {2}", DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff"), count, title);
                sb.AppendLine();
                for (int i = offset; i < offset + count; i++)
                {
                    sb.AppendFormat("{0:X2} ", buffer[i]);
                    if ((i - offset + 1) % 16 == 0) sb.AppendLine();
                }
                sb.AppendLine();
                string s = sb.ToString();
                Debug.Print(s);
                if (!_enabled) return;
                lock (_locker)
                {
                    File.AppendAllText(FilePath, s + Environment.NewLine, Encoding.UTF8);
                }
            }
            catch (Exception ex)
            {
                LogException(ex, "LogHex error");
            }
        }
    }
}