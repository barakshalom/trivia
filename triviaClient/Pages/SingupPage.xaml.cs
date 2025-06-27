using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
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
    // Sign-up page where users can create a new account by entering their details.
    public partial class SingupPage : Page
    {
        private ServerSocket _sock;

        // Constructor – receives the socket connection and initializes the page.
        public SingupPage(ServerSocket serverSocket)
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

        // Called when the "Signup" button is clicked.
        // Validates the fields, attempts to register the user via the server,
        // and navigates to the main menu on success.
        private void SingupButton_Click(object sender, RoutedEventArgs e)
        {
            // Validate that no fields are empty.
            if (string.IsNullOrEmpty(UsernameBox.txtInput.Text) || string.IsNullOrEmpty(PasswordBox.txtInput.Text) || string.IsNullOrEmpty(EmailBox.txtInput.Text) || string.IsNullOrEmpty(AddressBox.txtInput.Text) || string.IsNullOrEmpty(PhonesBox.txtInput.Text) || string.IsNullOrEmpty(Datebox.txtInput.Text))
            {
                ErrorText.Text = "Signup failed: One of more fields are empty.";
            }

            // Attempt to create a new account
            if (_sock.Signup(UsernameBox.txtInput.Text, PasswordBox.txtInput.Text, EmailBox.txtInput.Text, AddressBox.txtInput.Text, PhonesBox.txtInput.Text, Datebox.txtInput.Text))
            {
                ErrorText.Text = "Signup successful!";
                _sock.setUsername(UsernameBox.txtInput.Text);
                this.NavigationService.Navigate(new MenuPage(_sock));
            }
        }
    }
}
