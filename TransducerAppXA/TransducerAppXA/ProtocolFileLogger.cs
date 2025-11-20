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
        /// Evento disparado sempre que uma entrada é escrita (mesmo que a gravação em disco falhe).
        /// direction: "TX" / "RX" / "SYS" / etc.
        /// text: mensagem textual (pode conter o telegrama entre colchetes)
        /// raw: bytes (pode ser null)
        /// </summary>
        public static event Action<string, string, byte[]> OnLogWritten;

        private static void EnsureInitialized()
        {
            if (_initialized) return;

            lock (_lock)
            {
                if (_initialized) return;
                try
                {
                    // Tenta escolher um diretório confiável para gravação:
                    // Em Android: Environment.SpecialFolder.Personal resolve para o diretório interno do app.
                    // Em Desktop: usa AppDomain.CurrentDomain.BaseDirectory.
                    string baseDir = null;
                    try
                    {
                        baseDir = System.Environment.GetFolderPath(System.Environment.SpecialFolder.Personal);
                        if (string.IsNullOrWhiteSpace(baseDir))
                            baseDir = AppDomain.CurrentDomain.BaseDirectory;
                    }
                    catch
                    {
                        // fallback se houver qualquer problema
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

                    // cria o arquivo inicial (registro de início)
                    try
                    {
                        File.AppendAllText(_filePath, $"{DateTime.Now:yyyy-MM-dd HH:mm:ss.fff} - Protocol log started{System.Environment.NewLine}", Encoding.UTF8);
                    }
                    catch
                    {
                        // se falhar ao criar, deixamos _filePath com valor mas teremos exceção ao gravar;
                        // não interromper a execução: continuamos e deixamos _filePath para tentativa futura.
                    }
                }
                catch (Exception ex)
                {
                    // Se não conseguir criar pasta/arquivo, deixamos _filePath = null e prosseguimos.
                    try { System.Diagnostics.Debug.Print("ProtocolFileLogger init error: " + ex.Message); } catch { }
                    _filePath = null;
                }
                finally
                {
                    // Marcar inicializado para não ficar tentando infinitamente
                    _initialized = true;
                }
            }
        }

        /// <summary>
        /// Escreve log: tenta gravar em arquivo (se possível) e SEMPRE dispara OnLogWritten.
        /// direction: "TX"/"RX"/"SYS" etc.
        /// text: representação textual (telegrama, descrição)
        /// raw: bytes (opcional)
        /// </summary>
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

                // Tenta gravar em arquivo se _filePath válido
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
                        // Não interromper a aplicação. Log para debug e continuar.
                        try { System.Diagnostics.Debug.Print("ProtocolFileLogger write error: " + exFile.Message); } catch { }
                    }
                }
                else
                {
                    // fallback: escrever no Debug output para desenvolvedor
                    try { System.Diagnostics.Debug.Print(formatted); } catch { }
                }

                // Dispara evento para UI, sempre (não depende de escrita em arquivo)
                try
                {
                    OnLogWritten?.Invoke(direction, text ?? "", raw);
                }
                catch
                {
                    // não deixar a falha do handler quebrar o app
                }
            }
            catch (Exception ex)
            {
                try { System.Diagnostics.Debug.Print("ProtocolFileLogger.WriteProtocol error: " + ex.Message); } catch { }
                // Mesmo em caso de erro inesperado, ainda tentar notificar UI com a mensagem de erro
                try
                {
                    OnLogWritten?.Invoke("SYS", "ProtocolFileLogger internal error: " + ex.Message, null);
                }
                catch { }
            }
        }

        /// <summary>
        /// Formata bytes em HEX (para exibição em UI).
        /// </summary>
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