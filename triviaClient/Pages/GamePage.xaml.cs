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
using System.Windows.Media.Animation;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using trivia_gui.View.UserControls;
using System.Windows.Threading;
using System.Text.Json;

namespace trivia_gui.Pages
{
    // Page that handles the gameplay screen for the trivia game.
    // Loads questions one by one, sets timers, and manages answers.
    public partial class GamePage : Page
    {
        // Model that represents a single question and its possible answers.
        public class QuestionModel
        {
            public string Question { get; set; }
            public List<AnswerModel> Answers { get; set; }
        }


        private ServerSocket _sock;
        private int _questionsTime; // Time (in seconds) allowed per question.
        private DispatcherTimer _questionTimer; // Timer to count down per question.

        public GamePage(ServerSocket sock, int time) {
            _sock = sock;
            _questionsTime = time;
            InitializeComponent();
            LoadMockQuestions();  // Start game by loading the first question.
        }

        // Retrieves the next question from the server and parses it into local model.
        // If a question is available, it displays it using HandleQuestion().
        private void LoadMockQuestions() {
            // Creating a list of all the questions of the game
            var question = _sock.getQuestion();
            if (question.Item1) // Success flag
            {
                var questionText = question.Item2[1][1].GetString(); // Extract question text
                var answersArray = question.Item2[2]; // Extract answers array.
                var q = new QuestionModel
                {
                        Question = questionText,
                        Answers = answersArray.EnumerateArray().Select(answer => new AnswerModel
                        {
                            Text = answer[1].GetString(),
                            ansId = answer[0].GetInt32(),
                            IsCorrect = answer[0].GetInt32() == 0 // Server marks correct answer with id = 0.
                        }).ToList()
                };
                HandleQuestion(q);
            }

        }

        // Displays the given question on the screen, sets up answers,
        // and starts a countdown timer.
        private void HandleQuestion(QuestionModel question)
        {
            // Stop any previous timer.
            _questionTimer?.Stop();

            var questionLayout = new QuestionLayout();
            questionLayout.QuestionPlaceholder = question.Question;

            // Add each answer button to the layout.
            for (int i = 0; i < question.Answers.Count; i++)
            {
                var ans = question.Answers[i];
                questionLayout.AddAnswerToGrid(ans, i);
            }

            // Hook up answer selection event.
            questionLayout.AnswerChosen += OnAnswerChosen;

            // Display the layout in the page.
            ContentGrid.Children.Clear();
            ContentGrid.Children.Add(questionLayout);

            // Start countdown.
            int timeLeft = _questionsTime;
            questionLayout.SetTimerText(timeLeft.ToString());

            _questionTimer = new DispatcherTimer();
            _questionTimer.Interval = TimeSpan.FromSeconds(1);
            _questionTimer.Tick += (s, e) =>
            {
                timeLeft--;
                questionLayout.SetTimerText(timeLeft.ToString());
                try
                {
                    // If time runs out, submit a default answer.
                    if (timeLeft <= 0)
                    {
                        submitAnswer(0);
                    }

                }
                catch(Exception exc) { }
            };
            _questionTimer.Start();
        }

        // Handles user selecting an answer.
        private void OnAnswerChosen(int answerId)
        {
            submitAnswer(answerId);
        }

        // Submits the user's answer to the server.
        // If the game continues, load the next question.
        // Otherwise, navigate to results page.
        private void submitAnswer(int answerId)
        {
            if (_sock.submitAnswer(answerId))
            {
                // Load the next question.
                LoadMockQuestions();
            }
            else
            {
                // Game over – go to results page.
                this.NavigationService.Navigate(new GameResultPage(_sock));
                _questionTimer?.Stop();
            }

        }

        // Navigate back to the main menu when the "Back" button is clicked.
        private void BackToMenu_Click(object sender, RoutedEventArgs e)
        {
            _sock.leaveGame();
            this.NavigationService.Navigate(new MenuPage(_sock));
        }
    }
}
