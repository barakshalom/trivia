using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
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
using trivia_gui.Pages;
using System.Text.Json;
using System.IO;

namespace trivia_gui
{
    // Represents the login screen where users enter their username and password
    // to access their trivia game account.
    public partial class LoginPage : Page
    {
        private ServerSocket _sock;

        // Constructor – receives an open socket connection to communicate with the server.
        public LoginPage(ServerSocket serverSocket)
        {
            InitializeComponent();
            _sock = serverSocket;
        }

        // Navigate back to the main menu when the "Back" button is clicked.
        private void BackToMenu_Click(object sender, RoutedEventArgs e)
        {
            if (this.NavigationService.CanGoBack)
            {
                this.NavigationService.GoBack();
            }
        }

        // Called when the "Login" button is clicked.
        // Validates the fields and attempts to log in the user via the server.
        // On success, navigates to the main menu.
        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            // Check if any fields are empty.
            if (string.IsNullOrEmpty(UsernameBox.txtInput.Text) || string.IsNullOrEmpty(PasswordBox.txtInput.Text))
            {
                ErrorText.Text = "Login failed: One of more fields are empty.";
            }

            // Try to log in with the provided credentials.
            if (_sock.Login(UsernameBox.txtInput.Text, PasswordBox.txtInput.Text))
            {
                // Login succeeded.
                ErrorText.Text = "Login successful!";
                _sock.setUsername(UsernameBox.txtInput.Text);
                this.NavigationService.Navigate(new MenuPage(_sock));
            }
            else
            {
                // Login failed.
                ErrorText.Text = "Login failed: Incorrect username or password.";
            }
        }

        // Optional event hook for when the page finishes loading.
        // Currently unused.
        private void PageBase_Loaded(object sender, RoutedEventArgs e)
        {

        }
    }
}
