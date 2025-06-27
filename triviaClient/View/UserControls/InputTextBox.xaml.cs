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
    // Interaction logic for InputTextBox.xaml
    public partial class InputTextBox : UserControl
    {
        public InputTextBox()
        {
            InitializeComponent();
        }

        // Backing field for placeholder text
        private string placeholder;

        // Public property for placeholder text (shown when the input is empty).
        public string Placeholder
        {
            get { return placeholder; }
            set { placeholder = value; tbPlaceholder.Text = placeholder; } // Update the placeholder TextBlock content
        }

        // Event handler: triggered when the user types in the input box
        private void txtInput_TextChanged(object sender, TextChangedEventArgs e)
        {
            // Show placeholder if input is empty, otherwise hide it.
            if (string.IsNullOrEmpty(txtInput.Text))
            {
                tbPlaceholder.Visibility = Visibility.Visible;
            }
            else
            {
                tbPlaceholder.Visibility = Visibility.Hidden;
            }
        }
    }
}
