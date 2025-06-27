using System.Configuration;
using System.Data;
using System.Windows;
using trivia_gui.Pages;

namespace trivia_gui
{
    // Interaction logic for App.xaml
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);

            // Attach global exception handler to catch unhandled exceptions in the ui thread.
            this.DispatcherUnhandledException += App_DispatcherUnhandledException;
        }

        private void App_DispatcherUnhandledException(object sender, System.Windows.Threading.DispatcherUnhandledExceptionEventArgs e)
        {
            // If main window is open, call its cleanup logic.
            if (Current.MainWindow is MainWindow mainWindow)
            {
                mainWindow.closingGUI();
            }

            // Mark exception as handled and terminate the app.
            e.Handled = true;
            Environment.Exit(1);
        }

        protected override void OnExit(ExitEventArgs e)
        {
            // Ensure resources are cleaned up when the app exits normally.
            if (Current.MainWindow is MainWindow mainWindow)
            {
                mainWindow.closingGUI();
            }
            base.OnExit(e);
        }
    }
}
