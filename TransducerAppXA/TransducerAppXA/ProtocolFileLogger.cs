using System;
using System.IO;
using System.Text;

namespace TransducerAppXA
{
    /// <summary>
    /// Logger de protocolo: grava em arquivo quando possível e sempre dispara OnLogWritten para UI.
    /// - Usa diretório seguro para Android (Environment.SpecialFolder.Personal) quando disponível.
    /// - Não bloqueia a aplicação se a gravação falhar.
    /// - OnLogWritten é sempre chamado (event handlers na UI exibirão tudo).
    /// </summary>
    public static class ProtocolFileLogger
    {
        private static readonly object _lock = new object();
        private static bool _initialized = false;
        private static string _filePath = null;

        /// <summary>
        /// Event: direction, text, raw
        /// </summary>
        public static event Action<string, string, byte[]> OnLogWritten;

        /// <summary>
        /// Expose the current file path (readonly). Useful for debugging in UI.
        /// </summary>
        public static string FilePath
        {
            get
            {
                EnsureInitialized();
                return _filePath;
            }
        }

        private static void EnsureInitialized()
        {
            if (_initialized) return;

            lock (_lock)
            {
                if (_initialized) return;
                try
                {
                    string baseDir = null;
                    try
                    {
                        baseDir = System.Environment.GetFolderPath(System.Environment.SpecialFolder.Personal);
                        if (string.IsNullOrWhiteSpace(baseDir))
                            baseDir = AppDomain.CurrentDomain.BaseDirectory;
                    }
                    catch
                    {
                        baseDir = AppDomain.CurrentDomain.BaseDirectory;
                    }

                    string logsDir = Path.Combine(baseDir, "Logs");
                    if (!Directory.Exists(logsDir))
                    {
                        Directory.CreateDirectory(logsDir);
                    }

                    string ts = DateTime.Now.ToString("yyyyMMdd_HHmmss");
                    string fileName = $"Log-Protocol-{ts}.Log";
                    _filePath = Path.Combine(logsDir, fileName);

                    try
                    {
                        File.AppendAllText(_filePath, $"{DateTime.Now:yyyy-MM-dd HH:mm:ss.fff} - Protocol log started{System.Environment.NewLine}", Encoding.UTF8);
                    }
                    catch
                    {
                        // ignore file creation error; _filePath left for attempts later
                    }
                }
                catch (Exception ex)
                {
                    try { System.Diagnostics.Debug.Print("ProtocolFileLogger init error: " + ex.Message); } catch { }
                    _filePath = null;
                }
                finally
                {
                    _initialized = true;
                }
            }
        }

        public static void WriteProtocol(string direction, string text, byte[] raw = null)
        {
            try
            {
                EnsureInitialized();

                var sb = new StringBuilder();
                sb.AppendFormat("{0:yyyy-MM-dd HH:mm:ss.fff} [{1}] {2}", DateTime.Now, direction, text ?? "");
                sb.AppendLine();

                if (raw != null && raw.Length > 0)
                {
                    sb.Append("HEX: ");
                    sb.Append(ByteArrayToHexString(raw, " "));
                    sb.AppendLine();
                }

                sb.AppendLine(new string('-', 80));
                string formatted = sb.ToString();

                if (!string.IsNullOrEmpty(_filePath))
                {
                    try
                    {
                        lock (_lock)
                        {
                            File.AppendAllText(_filePath, formatted, Encoding.UTF8);
                        }
                    }
                    catch (Exception exFile)
                    {
                        try { System.Diagnostics.Debug.Print("ProtocolFileLogger write error: " + exFile.Message); } catch { }
                    }
                }
                else
                {
                    try { System.Diagnostics.Debug.Print(formatted); } catch { }
                }

                try
                {
                    OnLogWritten?.Invoke(direction, text ?? "", raw);
                }
                catch
                {
                    // swallow handler exceptions
                }
            }
            catch (Exception ex)
            {
                try { System.Diagnostics.Debug.Print("ProtocolFileLogger.WriteProtocol error: " + ex.Message); } catch { }
                try
                {
                    OnLogWritten?.Invoke("SYS", "ProtocolFileLogger internal error: " + ex.Message, null);
                }
                catch { }
            }
        }

        public static string ByteArrayToHexString(byte[] buffer, string separator = " ")
        {
            if (buffer == null || buffer.Length == 0) return string.Empty;
            var sb = new StringBuilder(buffer.Length * 3);
            for (int i = 0; i < buffer.Length; i++)
            {
                sb.AppendFormat("{0:X2}", buffer[i]);
                if (i < buffer.Length - 1) sb.Append(separator);
                if ((i + 1) % 16 == 0) sb.AppendLine();
            }
            return sb.ToString();
        }
    }
}