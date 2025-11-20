using System;
using System.IO;
using System.Text;
using System.Threading;

namespace TransducerAppXA
{
    // Logger simples e thread-safe usado pelo projeto.
    // Ajustes:
    // - Garante caminho válido (fallbacks para SpecialFolder.Personal / Temp)
    // - Cria diretório antes de tentar escrever
    // - Trata exceções de IO para não quebrar a aplicação
    // - Pequeno retry ao gravar para reduzir problemas de concorrência em dispositivos lentos
    public static class TransducerLogger
    {
        private static readonly object _locker = new object();
        private static bool _enabled = true;
        private static string _folder = null;
        public static string FilePath { get; private set; }

        // Configure: define pasta de logs (ou arquivo) e habilita/desabilita
        // Se folderOrFilePath for null/empty, usa um caminho padrão:
        //  - tenta Environment.SpecialFolder.LocalApplicationData
        //  - se não existir, usa Environment.SpecialFolder.Personal (Android)
        //  - se ainda não existir, usa Path.GetTempPath()
        public static void Configure(string folderOrFilePath, bool enabled = true)
        {
            _enabled = enabled;

            try
            {
                if (string.IsNullOrWhiteSpace(folderOrFilePath))
                {
                    // escolhe pasta padrão
                    folderOrFilePath = GetDefaultLogFolder();
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

                // garante folder válido; se algo falhar, troca para fallback
                EnsureFolderExists(_folder);

                // cria arquivo inicial de forma segura
                SafeAppendLine($"{DateTime.Now:yyyy-MM-dd HH:mm:ss.fff} - TransducerLogger configured");
            }
            catch (Exception ex)
            {
                // não lançar — apenas escreve em Debug e tenta fallback
                try
                {
                    DebugPrint("TransducerLogger.Configure failed: " + ex);
                    // fallback
                    string fallback = Path.Combine(GetDefaultLogFolder(), "transducer_debug.log");
                    _folder = Path.GetDirectoryName(fallback);
                    FilePath = fallback;
                    EnsureFolderExists(_folder);
                    SafeAppendLine($"{DateTime.Now:yyyy-MM-dd HH:mm:ss.fff} - TransducerLogger configured (fallback)");
                }
                catch
                {
                    // swallow
                }
            }
        }

        // Log básico (uma linha)
        public static void Log(string message)
        {
            if (!_enabled) return;
            try
            {
                string line = $"{DateTime.Now:yyyy-MM-dd HH:mm:ss.fff} - {message}";
                DebugPrint(line);
                SafeAppendLine(line);
            }
            catch
            {
                // não propaga exceções de logging
            }
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
                DebugPrint(line);
                SafeAppendLine(line);
            }
            catch
            {
                // swallow
            }
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
                DebugPrint(s);
                if (!_enabled) return;
                SafeAppendLine(s);
            }
            catch (Exception ex)
            {
                try { LogException(ex, "LogHex error"); } catch { }
            }
        }

        // --- Helpers ---

        // Retorna pasta padrão para logs (sem o nome do arquivo)
        private static string GetDefaultLogFolder()
        {
            try
            {
                // tenta pasta LocalApplicationData (Windows / Xamarin iOS/Android compatível em muitos casos)
                string p = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
                if (!string.IsNullOrEmpty(p) && Directory.Exists(p))
                {
                    return Path.Combine(p, "transducerapp", "logs");
                }

                // fallback para SpecialFolder.Personal (Android)
                p = Environment.GetFolderPath(Environment.SpecialFolder.Personal);
                if (!string.IsNullOrEmpty(p) && Directory.Exists(p))
                {
                    return Path.Combine(p, "transducerapp", "logs");
                }

                // último recurso: Temp
                p = Path.GetTempPath();
                return Path.Combine(p, "transducerapp", "logs");
            }
            catch
            {
                // se tudo der errado, usa current directory
                try
                {
                    return Path.Combine(Directory.GetCurrentDirectory(), "transducerapp", "logs");
                }
                catch
                {
                    // fallback definitivo
                    return "transducerapp_logs";
                }
            }
        }

        // Garante que a pasta existe (cria se não existir)
        private static void EnsureFolderExists(string folder)
        {
            if (string.IsNullOrEmpty(folder)) return;
            try
            {
                if (!Directory.Exists(folder))
                {
                    Directory.CreateDirectory(folder);
                }
            }
            catch (Exception ex)
            {
                // tenta criar utilizando fallback
                DebugPrint("EnsureFolderExists error: " + ex);
                string fallback = GetDefaultLogFolder();
                try
                {
                    if (!Directory.Exists(fallback))
                        Directory.CreateDirectory(fallback);
                    _folder = fallback;
                    FilePath = Path.Combine(_folder, Path.GetFileName(FilePath ?? "transducer_debug.log"));
                }
                catch { }
            }
        }

        // Append com retry (evita falhas por concorrência no Android/emulador)
        private static void SafeAppendLine(string line, int retries = 3, int delayMs = 50)
        {
            if (string.IsNullOrEmpty(FilePath)) return;
            for (int attempt = 0; attempt < retries; attempt++)
            {
                try
                {
                    lock (_locker)
                    {
                        // garante que pasta exista antes de escrever
                        try
                        {
                            var dir = Path.GetDirectoryName(FilePath);
                            if (!string.IsNullOrEmpty(dir) && !Directory.Exists(dir))
                                Directory.CreateDirectory(dir);
                        }
                        catch { }

                        File.AppendAllText(FilePath, line + Environment.NewLine, Encoding.UTF8);
                    }
                    return;
                }
                catch (Exception ex)
                {
                    // último intento: registra em Debug e espera antes de tentar novamente
                    DebugPrint($"SafeAppendLine attempt {attempt + 1} failed: {ex.Message}");
                    Thread.Sleep(delayMs);
                }
            }
            // se todos falharem, grava no Debug apenas
            DebugPrint("SafeAppendLine: all attempts failed, dropping log line.");
        }

        // Método interno para escrever no debug output (não usa System.Diagnostics.Debug para evitar conflitos em alguns ambientes)
        private static void DebugPrint(string s)
        {
            try
            {
#if WINDOWS_UWP
                System.Diagnostics.Debug.WriteLine(s);
#else
                System.Diagnostics.Debug.Print(s);
#endif
            }
            catch { }
        }
    }
}