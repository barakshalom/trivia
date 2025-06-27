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

namespace trivia_gui.View.UserControls
{
    // Interaction logic for QuestionLayout.xaml

    // Simple data model representing a possible answer to a question.
    public class AnswerModel
    {
        public string Text { get; set; }    // The text displayed on the answer button
        public int ansId { get; set; }      // The id of the answer
        public bool IsCorrect { get; set; } // Whether this answer is the correct one
    }

    // UserControl used to display a question and its possible answers in the ui
    public partial class QuestionLayout : UserControl
    {
        // Event raised when the user clicks an answer button (sends selected answer id + 1)
        public event Action<int> AnswerChosen;

        private string questionPlaceholder;

        // The question text shown at the top of the layout.
        public string QuestionPlaceholder
        {
            get { return questionPlaceholder; }
            set { questionPlaceholder = value; tbQuestionPlaceholder.Text = questionPlaceholder; } // Update the textblock in the ui
        }

        // Constructor: initializes ui components.
        public QuestionLayout()
        {
            InitializeComponent();
        }

        // Dynamically adds an answer button to the grid in the correct row.
        public void AddAnswerToGrid(AnswerModel answer, int row)
        {
            Button answerButton = new Button
            {
                Content = answer.Text,
                Tag = answer.ansId,
                Style = (Style)FindResource("MenuButton")
            };

            // When clicked, raise AnswerChosen with (answerId + 1).
            answerButton.Click += (s, e) =>
            {
                int selectedAnswerId = (int)((Button)s).Tag;
                AnswerChosen?.Invoke(selectedAnswerId + 1);
            };

            // Position the button on the grid (row + 1 to offset from question row).
            Grid.SetRow(answerButton, row + 1);
            QuestionGrid.Children.Add(answerButton);
        }

        // Updates the countdown timer text shown in the question layout.
        public void SetTimerText(string time)
        {
            tbTimer.Text = $"Time left: {time}";
        }
    }
}
