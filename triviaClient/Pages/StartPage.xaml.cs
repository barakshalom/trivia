using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace trivia_gui.Pages
{
    // The first screen of the trivia game.
    // Offers two options: log in or sign up.
    // You must be logged in to access any game features.
    public partial class StartPage : Page
    {
        private ServerSocket _sock;

        public StartPage(ServerSocket socket)
        {
            InitializeComponent();
            _sock = socket;
        }

        // Navigates to the login page when "Login" is clicked.
        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new LoginPage(_sock));
        }

        // Navigates to the signup page when "Signup" is clicked.
        private void SignupButton_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new SingupPage(_sock));
        }

        // Closes the socket and exits the application when "Exit" is clicked.
        private void ExitButton_Click(object sender, RoutedEventArgs e)
        {
            _sock.Close();
            Environment.Exit(0);
        }
    }
}
