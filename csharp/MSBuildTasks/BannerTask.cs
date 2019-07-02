using System;
using System.IO;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

public class BannerTask : Task
{
    int _height;
    int _width;

    ConsoleCancelEventHandler _sigintDelegate = new ConsoleCancelEventHandler(HandleSigInt);
    FileStream _fileStream;
    StreamReader _filein;
    bool _exitConsole;
    bool _taskSucceeded;

    static bool _sigintReceived;
    static StreamWriter _stdout;

    ITaskItem _textFileItem;

   [Required]
    public ITaskItem TextFile { get; set; }

    public override bool Execute()
    {
        try
        {
            _textFileItem = TextFile;
            if (_textFileItem == null || _textFileItem.ItemSpec.Length <= 0)
            {
                throw new FileNotFoundException("Invalid TaskItem passed to BannerTask::TextFile");
            }

            _exitConsole = false; ;
            _sigintReceived = false;
            _taskSucceeded = true;
            using (_fileStream = new FileStream(_textFileItem.ItemSpec, FileMode.Open, FileAccess.Read))
            using (_filein = new StreamReader(_fileStream))
            using (_stdout = new StreamWriter(Console.OpenStandardOutput()))
            {
                ConsoleKeyInfo cki;
                Console.Clear();
                Console.CancelKeyPress += _sigintDelegate;

                _height = Console.BufferHeight;
                _width = Console.BufferWidth;

                _stdout.AutoFlush = true;
                if (_width >= 80)
                {
                    _stdout.Write(_filein.ReadToEnd());
                }

                while (!_exitConsole)
                {
                    cki = Console.ReadKey();
                    _stdout.Write(" --- You pressed: ");
                    if ((cki.Modifiers & ConsoleModifiers.Alt) != 0) Console.Write("ALT-");
                    if ((cki.Modifiers & ConsoleModifiers.Shift) != 0) Console.Write("SHIFT-");
                    if ((cki.Modifiers & ConsoleModifiers.Control) != 0) Console.Write("CTRL-");
                    _stdout.WriteLine(cki.Key.ToString());

                    if (_sigintReceived) throw new ApplicationException("Terminating due to SIGINT");
                    if (cki.Key == ConsoleKey.Escape) _exitConsole = true;
                }
            }
        }
        catch (Exception e)
        {
            Log.LogError("SockLynx Build Error: " + e.Message);
            _taskSucceeded = false;
        }
        finally
        {
            Console.CancelKeyPress -= _sigintDelegate;
        }

        return _taskSucceeded;
    }

    static void HandleSigInt(object sender, ConsoleCancelEventArgs args)
    {
        args.Cancel = true;
        _sigintReceived = true;
        _stdout.WriteLine("");
        _stdout.WriteLine("  Key pressed: {0}", args.SpecialKey);
        _stdout.WriteLine("  Cancel property: {0}\n\n", args.Cancel);
    }
}
