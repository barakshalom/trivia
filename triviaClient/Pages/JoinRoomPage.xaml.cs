using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
using trivia_gui.Managers;
using trivia_gui.Pages;
using trivia_gui.View.UserControls;

namespace trivia_gui.Pages
{
    // Displays all currently open trivia rooms.
    // Shows player counts and lets the user choose a room to join.

    // Small data holder for room metadata returned by the server.
    public class Room
    {
        private int roomId { get; set; }
        private int maxPlayers { get; set; }
        private string name { get; set; }

        public string GetName()
        {
            return name;
        }

        public Room(int roomId, string roomName, int maxPlayers)
        {
            name = roomName;
            this.roomId = roomId;
            this.maxPlayers = maxPlayers;
        }

        public int getId()
        {
            return roomId;
        }
        public int getMaxPlayers()
        {
            return maxPlayers;
        }
    }

    public partial class JoinRoomPage : Page, IRefreshable
    {
        // List bound to the ui, refreshed every time we poll the server.
        public List<Room> Rooms { get; set; } = new List<Room>();
        private ServerSocket _sock;
        public JoinRoomPage(ServerSocket socket)
        {
            InitializeComponent();
            _sock = socket;
            DataContext = this; // Make Rooms available to XAML bindings (if any)
            Refresh(); // Initial population
        }

        // Refresh logic

        // Re queries the server for open rooms, clears the visual stack panel,
        // and rebuilds it with up-to-date information.
        public void Refresh()
        {
            if (!this.IsVisible)
                // If page is not visible, skip work
                return;
            Rooms.Clear();
            RoomsStackPanel.Children.Clear();
            try
            {
                var rooms = _sock.getRooms();
                for (int i = 0; i < rooms.Count; i++)
                {
                    var room = rooms[i];
                    var roomId = room["id"];
                    var roomName = room["name"].ToString();
                    var maxPlayers = room["maxPlayers"];

                    // Add to local collection.
                    Rooms.Add(new Room(int.Parse(roomId.ToString()), roomName, int.Parse(maxPlayers.ToString())));

                    // Query current player count in room.
                    var players = _sock.getPlayers(roomId);
                    AddRoomToGrid(roomName, $"{players?["PlayersInRoom"].Count}/{maxPlayers}", i);
                }
            } catch 
            {
                ErrorText.Text = "Couldn't Open Room Try Again.";
            }
        }

        //  UI helpers

        // Creates a RoomPreview control, wires its event, and adds it to the vertical stack.
        private void AddRoomToGrid(string name, string activity, int row)
        {
            var room = new RoomPreview
            {
                RoomName = name,
                RoomActivity = activity,
                Tag = name // store name for quick lookup
            };

            room.RoomJoinRequested += HandleJoinRoom;

            RoomsStackPanel.Children.Add(room);
        }

        // Event handler when user clicks "Join" on a RoomPreview.
        // Validates capacity and sends join request to server.
        private void HandleJoinRoom(object sender, EventArgs e)
        {
            var preview = sender as RoomPreview;
            if (preview == null) return;

            // Check capacity before sending join request
            string roomName = preview.Tag as string;
            var roomStat = getRoom(roomName);
            if (_sock.getPlayers(roomStat.getId()).Count <= roomStat.getMaxPlayers() - 1 && _sock.joinRoom(roomStat.getId()))
            {
                this.NavigationService.Navigate(new RoomPage(_sock, roomName));
            }
        }

        // Navigate back to the main menu when the "Back" button is clicked.
        private void BackToMenu_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new MenuPage(_sock));
        }

        // Retrieve the Room object by name from Rooms list.
        public Room getRoom(string roomName)
        {
            for (int i = 0; i < Rooms.Count; i++)
            {
                if (roomName == Rooms[i].GetName())
                {
                    return Rooms[i];
                }
            }
            return null;
        }
    }
}