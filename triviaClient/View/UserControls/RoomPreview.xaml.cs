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
using trivia_gui.Pages;

namespace trivia_gui.View.UserControls
{
    // Interaction logic for RoomPreview.xaml
    public partial class RoomPreview : UserControl
    {
        // Event triggered when the user clicks the "Join" button.
        public event EventHandler RoomJoinRequested;

        public RoomPreview()
        {
            InitializeComponent(); // Loads the xaml content for the control.
        }

        // Backing fields for room name and activity state.
        private string roomName;
        private string roomActivity;

        // Public property for the room name (binds to RoomN text block in xaml)
        public string RoomName {
            get { return roomName; }
            set { roomName = value; RoomN.Text = roomName; } // Update uo element with new value 
        }

        // Public property for room activity (binds to RoomA text block in xaml).
        public string RoomActivity {
            get { return roomActivity; }
            set { roomActivity = value; RoomA.Text = roomActivity; } // Update ui element with new value.
        }

        // Called when the "Join" button is clicked.
        // Raises the RoomJoinRequested event to notify the parent page.
        private void JoinRoomButton_Click(object sender, RoutedEventArgs e)
        {
            RoomJoinRequested?.Invoke(this, EventArgs.Empty);
        }
    }
}
