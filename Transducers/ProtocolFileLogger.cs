using System;
using System.IO;
using System.Text;

namespace Transducers
{
    /// <summary>
    /// Logger de protocolo dedicado: cria um arquivo único por sessão chamado
    /// Logs\Log-Protocol-YYYYMMdd_HHmmss.Log e grava TX/RX completos (texto + hex).
    /// Uso: ProtocolFileLogger.WriteProtocol("TX", cmdout, txBytes);
    ///      ProtocolFileLogger.WriteProtocol("RX", lastPackageText, rawBytes);
    /// </summary>
    public static class ProtocolFileLogger
    {
        private static readonly object _lock = new object();
        private static bool _initialized = false;
        private static string _filePath = null;

        // Inicializa ao primeiro uso: cria pasta Logs e define nome do arquivo com timestamp.
        private static void EnsureInitialized()
        {
            if (_initialized) return;

            lock (_lock)
            {
                if (_initialized) return;
                try
                {
                    // Mesma pasta usada por StartLog(): AppDomain.CurrentDomain.BaseDirectory + "\\Logs"
                    string baseDir = AppDomain.CurrentDomain.BaseDirectory;
                    string logsDir = Path.Combine(baseDir, "Logs");
                    if (!Directory.Exists(logsDir))
                        Directory.CreateDirectory(logsDir);

                    string ts = DateTime.Now.ToString("yyyyMMdd_HHmmss");
                    string fileName = $"Log-Protocol-{ts}.Log";
                    _filePath = Path.Combine(logsDir, fileName);

                    // cria o arquivo inicial (registro de início)
                    File.AppendAllText(_filePath, $"{DateTime.Now:yyyy-MM-dd HH:mm:ss.fff} - Protocol log started{Environment.NewLine}", Encoding.UTF8);
                }
                catch (Exception ex)
                {
                    // não lançar: falhas de log não devem quebrar app
                    try { System.Diagnostics.Debug.Print("ProtocolFileLogger init error: " + ex.Message); } catch { }
                    _filePath = null;
                }
                _initialized = true;
            }
        }

        /// <summary>
        /// Escreve uma linha no arquivo de protocolo. Direction deve ser "TX" ou "RX".
        /// text: representação textual (telegrama). raw: bytes (pode ser null).
        /// </summary>
        public static void WriteProtocol(string direction, string text, byte[] raw = null)
        {
            try
            {
                EnsureInitialized();
                if (string.IsNullOrEmpty(_filePath))
                    return;

                var sb = new StringBuilder();
                sb.AppendFormat("{0:yyyy-MM-dd HH:mm:ss.fff} [{1}] {2}", DateTime.Now, direction, text ?? "");
                sb.AppendLine();

                if (raw != null && raw.Length > 0)
                {
                    sb.Append("HEX: ");
                    sb.Append(ByteArrayToHexString(raw, " "));
                    sb.AppendLine();
                }

                // separador visual entre mensagens
                sb.AppendLine(new string('-', 80));

                lock (_lock)
                {
                    File.AppendAllText(_filePath, sb.ToString(), Encoding.UTF8);
                }
            }
            catch (Exception ex)
            {
                try { System.Diagnostics.Debug.Print("ProtocolFileLogger.WriteProtocol error: " + ex.Message); } catch { }
            }
        }

        private static string ByteArrayToHexString(byte[] buffer, string separator = " ")
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