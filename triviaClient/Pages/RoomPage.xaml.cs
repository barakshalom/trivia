using System;
using System.Collections.Generic;
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
    // Displays an active game room.
    // Admins can start or close the room, users can leave.
    // Shows live participant list and room settings.

    public partial class RoomPage : Page, IRefreshable
    {
        private ServerSocket _sock;
        private bool _isAdmin;
        private int _answerTimeout;

        // Constructor – builds the room UI based on whether the current user is an admin.
        public RoomPage(ServerSocket socket, string roomName, string maxUsers = "", string questionCount = "", string answerTimeout = "", bool isAdmin = false)
        {
            InitializeComponent();

            _sock = socket;
            _isAdmin = isAdmin;
            TitleBox.Text = roomName;

            // Set up ui depending on role.
            if (_isAdmin)
                SetupAdminUI(maxUsers, questionCount, answerTimeout);
            else
                SetupUserUI();

            // Trigger initial participant and room info load.
            Loaded += (_, __) => Refresh();
        }

        // Shows room information and hides the 'Leave' button for admins.
        private void SetupAdminUI(string maxUsers, string questionCount, string answerTimeout)
        {
            RoomInfoBox.Text = $"max number players: {maxUsers} number of questions: {questionCount} time per question: {answerTimeout}";
            _answerTimeout = int.Parse(answerTimeout);
            LeaveButton.Visibility = Visibility.Collapsed;
        }

        // Hides admin-only controls for regular users.
        private void SetupUserUI()
        {
            CloseButton.Visibility = Visibility.Collapsed;
            StartGameButton.Visibility = Visibility.Collapsed;
        }

        // Polls the server for the latest room state.
        // Updates the player list and handles state transitions.
        public void Refresh()
        {
            const int TRUE = 1;
            if (!this.IsVisible)
                return;
            try
            {
                (int code, string message) = _sock.getState();
                if (code == (byte)Requests.STATE)
                {
                    // Deserialize room state.
                    var roomState = JsonSerializer.Deserialize<Dictionary<string, object>>(message);
                    var room = JsonSerializer.Deserialize<Dictionary<string, JsonElement>>(roomState["RoomState"].ToString());
                    var players = room["players"].EnumerateArray().Select(p => p.GetString()).ToList();
                    var questionsCount = room["answerCount"];
                    var answerTimeout = room["answerTimeout"];

                    // If room is still active.
                    if (room["status"].GetInt32() == TRUE)
                    {
                        // Remove old participant UI elements.
                        for (int i = Usernames.Children.Count - 1; i >= 0; i--)
                        {
                            if (Usernames.Children[i] is FrameworkElement elem && (string)elem.Tag == "participant")
                            {
                                Usernames.Children.RemoveAt(i);
                            }
                        }

                        // Add updated list of participants.
                        foreach (var player in players)
                        {
                            AddParticipant(player);
                        }

                        // If room info hasn't been set yet.
                        if (RoomInfoBox.Text == "")
                        {
                            RoomInfoBox.Text = $"current player count: {players.Count} number of questions: {questionsCount} time per question: {answerTimeout}";
                            _answerTimeout = int.Parse(answerTimeout.ToString());
                        }
                    }
                }
                else
                {
                    // Handle response to control actions like start/leave/close
                    var messageObj = JsonSerializer.Deserialize<Dictionary<string, int>>(message);
                    if (messageObj["status"] == TRUE)
                    {
                        if (code == (byte)Requests.START_ROOM)
                        {
                            _sock.Receive();
                            this.NavigationService.Navigate(new GamePage(_sock, _answerTimeout));
                        }
                        else if (code == (byte)Requests.LEAVE_ROOM || code == (byte)Requests.CLOSE_ROOM)
                        {
                            _sock.Receive();
                            this.NavigationService.Navigate(new MenuPage(_sock));
                        }
                    } 
                }
            }
            catch (Exception exception)
            {
            }
        }

        // Adds a participant's name to the UI grid.
        public void AddParticipant(string username)
        {
            TextBlock tb = new TextBlock
            {
                Text = username,
                Style = (Style)this.FindResource("BodyText"),
                Tag = "participant"
            };

            // Place the new participant below existing ones
            int participantCount = Usernames.Children
                .OfType<FrameworkElement>()
                .Count(e => (string)e.Tag == "participant");
            Grid.SetRow(tb, 3 + participantCount);
            Usernames.Children.Add(tb);
        }

        // Sends a leave or close command to the server.
        public void LeaveRoom(byte code)
        {
            //there is no point in putting it at sock cause i would just create 2 more functions
            _sock.Send(string.Empty, code);
        }

        // Event handlers using lambdas for compactness
        private void LeaveButton_Click(object sender, RoutedEventArgs e) => LeaveRoom((byte)Requests.LEAVE_ROOM);
        private void CloseButton_Click(object sender, RoutedEventArgs e) =>  LeaveRoom((byte)Requests.CLOSE_ROOM);
        private void BackToMenu_Click(object sender, RoutedEventArgs e) => LeaveRoom(_isAdmin ? (byte)Requests.CLOSE_ROOM : (byte)Requests.LEAVE_ROOM);

        // Checks if the current user is the room admin.
        public bool isAdmin() => _isAdmin;

        // Starts the game when admin clicks "Start".
        private void StartGameButton_Click(object sender, RoutedEventArgs e)
        {
            _sock.StartGame();
            this.NavigationService.Navigate(new GamePage(_sock, _answerTimeout));
        }
    }
}
