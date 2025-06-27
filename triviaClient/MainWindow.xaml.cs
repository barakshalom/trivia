using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using trivia_gui.Pages;

namespace trivia_gui
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private const int REFRESH_TIME = 1000;
        private ServerSocket _sock;
        private Thread _thread;
        private volatile bool _shouldStop = false; //using volatile we can use this between threads
        public Page? CurrentPage => MainFrame.Content as Page;
        public MainWindow()
        {
            InitializeComponent();
            Application.Current.MainWindow = this;
            try
            {
                _sock = new ServerSocket();
            }
            catch (Exception)
            {
                Environment.Exit(0);
                _shouldStop = true;
            }
            MainFrame.Navigate(new StartPage(_sock));
            this.Closing += Window_Closing;
            _thread = new Thread(() => refreshPage());
            _thread.Start();
        }

        public void refreshPage()
        {
            while (!_shouldStop)
            {
                try
                {
                    Dispatcher.Invoke(() =>
                    {
                        if(CurrentPage is IRefreshable refreshablePage)
                        {
                            refreshablePage.Refresh();
                        }
                    });
                    Thread.Sleep(REFRESH_TIME);
                }
                catch(Exception)
                {

                }
            }
        }
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            closingGUI();
        }
        public void closingGUI()
        {
            if (CurrentPage is RoomPage roomPage)
            {
                _sock.CloseRoom();
            }
            _sock.Logout();
            _sock.Close();
            _shouldStop = true;
        }
    }
}