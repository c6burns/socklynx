using System;
using Microsoft.Build.Utilities;
using Microsoft.Build.Framework;

namespace SimpleLogger
{

    public class MySimpleLogger : Logger
    {
        public override void Initialize(Microsoft.Build.Framework.IEventSource eventSource)
        {
            //Register for the ProjectStarted, TargetStarted, and ProjectFinished events
            eventSource.ProjectStarted += new ProjectStartedEventHandler(eventSource_ProjectStarted);
            eventSource.TargetStarted += new TargetStartedEventHandler(eventSource_TargetStarted);
            eventSource.ProjectFinished += new ProjectFinishedEventHandler(eventSource_ProjectFinished);
        }

        void eventSource_ProjectStarted(object sender, ProjectStartedEventArgs e)
        {
            Console.WriteLine("Project Started: " + e.ProjectFile);
        }

        void eventSource_ProjectFinished(object sender, ProjectFinishedEventArgs e)
        {
            Console.WriteLine("Project Finished: " + e.ProjectFile);
        }
        void eventSource_TargetStarted(object sender, TargetStartedEventArgs e)
        {
            if (Verbosity == LoggerVerbosity.Detailed)
            {
                Console.WriteLine("Target Started: " + e.TargetName);
            }
        }
    }
}
