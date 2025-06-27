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
    // Displays the top three users with the highest scores across all completed games.  
    // A call to the server retrieves the current leaderboard.
    public partial class BestScoresPage : Page
    {
        private ServerSocket _sock;
        public BestScoresPage(ServerSocket socket)
        {
            InitializeComponent();
            _sock = socket;

            // Retrieve an ordered list of usernames (length can vary).
            var scores = _sock.Scores();

            // Populate each label; fall back to "N/A" if the index
            // does not exist to prevent an out-of-range exception.
            User1.Text = scores.Count > 0 ? scores[0] : "N/A";
            User2.Text = scores.Count > 1 ? scores[1] : "N/A";
            User3.Text = scores.Count > 2 ? scores[2] : "N/A";
        }

       
        // Navigate back to the main menu when the "Back" button is clicked.
        private void BackToMenu_Click(object sender, RoutedEventArgs e)
        {
            if (this.NavigationService.CanGoBack)
            {
                this.NavigationService.Navigate(new MenuPage(_sock));

            }
        }

    }
}
