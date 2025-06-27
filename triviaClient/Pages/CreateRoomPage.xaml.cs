using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
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
using trivia_gui.Managers;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace trivia_gui.Pages
{
    // Allows the user to create a new trivia room and define

    public partial class CreateRoomPage : Page
    {
        // Hard limits enforced by both client and server
        private const int MAX_USERS = 10;
        private const int QUESTION_COUNT = 50;
        private const int TIMEOUT = 40;

        private ServerSocket _sock;

        public CreateRoomPage(ServerSocket socket)
        {
            InitializeComponent();
            _sock = socket;
        }

        // Navigate back to the main menu when the "Back" button is clicked.
        private void BackToMenu_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new MenuPage(_sock));
        }

        // Validate user input and if valid - create the room on the server.
        // On success, navigate straight into the newly created room.
        // (room creator is automatically the admin).
        private void CreateRoomButton_Click(object sender, RoutedEventArgs e)
        {

            // Check fields.
            if (// The regex ^\d+$ means: the entire string must be one or more digits (no negatives, no letters, no decimals).
                !Regex.IsMatch(NumOfPlayersBox.txtInput.Text, @"^\d+$") ||
                !Regex.IsMatch(NumOfQBox.txtInput.Text, @"^\d+$") ||
                !Regex.IsMatch(TimeForQBox.txtInput.Text, @"^\d+$") ||
                // These make sure no field is zero or negative. All inputs must be greater than 0.
                int.Parse(NumOfPlayersBox.txtInput.Text) > MAX_USERS || int.Parse(NumOfQBox.txtInput.Text) > QUESTION_COUNT || int.Parse(TimeForQBox.txtInput.Text) > TIMEOUT || int.Parse(NumOfQBox.txtInput.Text) <= 0 || int.Parse(NumOfPlayersBox.txtInput.Text) <= 0 || int.Parse(TimeForQBox.txtInput.Text) <= 0)
            {
                
                ErrorText.Text = "Create Room failed: One of more fields is not valid.";
            }
            else
            {
                // Attempt to create the room on the server.
                if (_sock.CreateRoom(NameBox.txtInput.Text, NumOfPlayersBox.txtInput.Text, NumOfQBox.txtInput.Text, TimeForQBox.txtInput.Text))
                {
                    // Success - show confirmation and navigate to the room page.
                    ErrorText.Text = "Room was created!";
                    var roomPage = new RoomPage(_sock, NameBox.txtInput.Text, NumOfPlayersBox.txtInput.Text, NumOfQBox.txtInput.Text, TimeForQBox.txtInput.Text, true);
                    this.NavigationService.Navigate(roomPage);
                }
            }

        }
    }
}
