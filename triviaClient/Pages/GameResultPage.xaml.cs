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
    // Model class to represent a single player's game result row.
    public class GameResultRow
    {
        // Model class to hold individual game result data per player.
        public string Username { get; set; }
        public int RightAnswers { get; set; }
        public double AverageTime { get; set; }

        public GameResultRow(string username,int rightAnswer, double averageTime ) 
        {
            this.Username = username;
            this.RightAnswers = rightAnswer;
            this.AverageTime = averageTime;
        }
    }

    // Page shown at the end of the trivia game, displaying the score,
    // number of correct answers, and average time per player.
    public partial class GameResultPage : Page, IRefreshable
    {
        private ServerSocket _sock;
        private List<GameResultRow> results; // Parsed game results for each player
        private bool isInGame; // Flag to check if player is still in a game

        public GameResultPage(ServerSocket sock)
        {
            _sock = sock;
            isInGame = true;
            InitializeComponent();
            LoadResults(); // Load and display the game results.
        }

        // Loads player results from the server, clears old grid entries,
        // parses the JSON data into C# objects, and updates the display.
        public void LoadResults()
        {
            const int MAX_CHILDREN = 6;  // Initial UI components (like headers).
            results = new List<GameResultRow>();

            // Reset previous dynamic rows (preserve initial controls).
            while (ResultsGrid.Children.Count > MAX_CHILDREN)
            {
                ResultsGrid.Children.RemoveAt(MAX_CHILDREN);
            }
            while (ResultsGrid.Children.Count > MAX_CHILDREN)
            {
                ResultsGrid.RowDefinitions.RemoveAt(ResultsGrid.RowDefinitions.Count - 1);
            }
            var playerResults = _sock.getResults();

            // Fetch results from the server
            foreach (JsonElement playerEntry in playerResults.EnumerateArray())
            {
                string username = playerEntry[0].GetString();
                var averageCount = playerEntry[1][1].GetDouble();
                var correctAns = playerEntry[2][1].GetInt32();
                results.Add(new GameResultRow(username, correctAns, averageCount));
            }
            LoadMockResults();
        }

        // Adds all result rows to the grid (excluding static headers).
        private void LoadMockResults()
        {
            // Creating a list of all the results of the game
            for (int i = 0; i < results.Count; i++)
            {
                // +2 to skip title/header rows in the grid layout
                addParticipantToGrid(results[i].Username, results[i].RightAnswers, results[i].AverageTime, i + 2); // +2 to skip headers
            }
        }

        // Adds a single player row (username, right answers, average time) to the grid.
        private void addParticipantToGrid(string username, int rightAnswers, double avgTime, int row)
        {
            // Username cell
            var usernameBlock = new TextBlock
            {
                Text = username,
                Style = (Style)FindResource("BodyText"),
                HorizontalAlignment = HorizontalAlignment.Center,
                VerticalAlignment = VerticalAlignment.Center
            };
            Grid.SetRow(usernameBlock, row);
            Grid.SetColumn(usernameBlock, 0);
            ResultsGrid.Children.Add(usernameBlock);

            // Correct answers cell
            var answersBlock = new TextBlock
            {
                Text = rightAnswers.ToString(),
                Style = (Style)FindResource("BodyText"),
                HorizontalAlignment = HorizontalAlignment.Center,
                VerticalAlignment = VerticalAlignment.Center
            };
            Grid.SetRow(answersBlock, row);
            Grid.SetColumn(answersBlock, 1);
            ResultsGrid.Children.Add(answersBlock);

            // Average time cell
            var avgTimeBlock = new TextBlock
            {
                Text = avgTime.ToString("0.00"), // Format to two decimal places
                Style = (Style)FindResource("BodyText"),
                HorizontalAlignment = HorizontalAlignment.Center,
                VerticalAlignment = VerticalAlignment.Center
            };
            Grid.SetRow(avgTimeBlock, row);
            Grid.SetColumn(avgTimeBlock, 2);
            ResultsGrid.Children.Add(avgTimeBlock);
        }

        // Navigate back to the main menu when the "Back" button is clicked.
        private void BackToMenu_Click(object sender, RoutedEventArgs e)
        {
            _sock.leaveGame();
            isInGame = false;
            this.NavigationService.Navigate(new MenuPage(_sock));
        }

        // Refreshes the results page if the player is still in the game and the page is visible.
        // This is useful when updating live scores or returning from background.
        public void Refresh()
        {
            if (!this.IsVisible)
                return;
            if (isInGame) LoadResults();
        }
    }
}

