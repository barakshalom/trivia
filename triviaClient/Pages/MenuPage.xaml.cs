using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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
    // Main menu page that provides navigation to all the core pages of the trivia game.
    // Users can create/join a room, check their stats, view top scores, or log out.
    public partial class MenuPage : Page
    {
        private ServerSocket _sock;
        public MenuPage(ServerSocket socket)
        {
            InitializeComponent();
            _sock = socket;
        }

        // Each button click navigates to a corresponding page in the application
        private void CreateRoomButton_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new CreateRoomPage(_sock));
        }

        private void MyStatusButton_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new MyStatusPage(_sock));
        }

        private void BestScoresButton_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new BestScoresPage(_sock));
        }

        private void LogoutButton_Click(object sender, RoutedEventArgs e)
        {
            _sock.Logout();
            this.NavigationService.Navigate(new StartPage(_sock));
        }

        private void JoinRoomButton_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new JoinRoomPage(_sock));
        }

        private void ExitButton_Click(object sender, RoutedEventArgs e)
        {
            _sock.Logout();
            _sock.Close();
            Environment.Exit(0);
        }
    }
}
