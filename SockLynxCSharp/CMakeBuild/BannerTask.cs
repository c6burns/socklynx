using System;
using System.IO;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

public class BannerTask : Task
{
    ITaskItem _textFileItem;
    FileStream _fileStream;
    StreamReader _filein;
    StreamWriter _stdout;
    bool _taskSucceeded;
    int _width;

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

            _taskSucceeded = true;
            using (_fileStream = new FileStream(_textFileItem.ItemSpec, FileMode.Open, FileAccess.Read))
            using (_filein = new StreamReader(_fileStream))
            using (_stdout = new StreamWriter(Console.OpenStandardOutput()))
            {
                _stdout.AutoFlush = true;
                _width = Console.BufferWidth;
                if (_width >= 80)
                {
                    _stdout.Write(_filein.ReadToEnd());
                }
            }
        }
        catch (Exception e)
        {
            Log.LogError("SockLynx Build Error: " + e.Message);
            _taskSucceeded = false;
        }

        return _taskSucceeded;
    }
}
