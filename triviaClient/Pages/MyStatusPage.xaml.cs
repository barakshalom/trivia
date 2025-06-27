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
    // Displays the current user's statistics in the trivia game:
    // number of games played, correct and wrong answers, and average answer time.
    public partial class MyStatusPage : Page
    {
        private ServerSocket _sock;

        // Constructor – fetches user statistics from the server and displays them.
        public MyStatusPage(ServerSocket socket)
        {
            InitializeComponent();
            _sock = socket;

            // Get statistics from the server (games played, correct/total answers, average time).
            (string totalGames, string correctAnswers, string totalAnswers, string timeAnswer) = _sock.Stats();

            // Calculate number of incorrect answers.
            int numberLoses = int.Parse(totalAnswers) - int.Parse(correctAnswers);

            // Display statistics in UI text fields.
            NumOfGames.Text = "Number of games: " + totalGames;
            NumOfRightAns.Text = "Number of right answers: " + correctAnswers;
            NumOfWrongAns.Text = "Number of wrong answers: " + numberLoses.ToString();
            aAvgTimeForAns.Text = "Average time for answer: " + timeAnswer;
        }

        // Navigate back to the main menu when the "Back" button is clicked.
        private void BackToMenu_Click(object sender, RoutedEventArgs e)
        {
            this.NavigationService.Navigate(new MenuPage(_sock));
        }
    }
}
