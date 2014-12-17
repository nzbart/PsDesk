using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PsDeskController
{
    public partial class MainForm : Form
    {
        private DeskController _controller;
        private AutoUpDown _autoUpDown;

        private delegate void StatusUpdateDelegate(string newStatus);

        public MainForm()
        {
            InitializeComponent();
            _controller = new DeskController();
            _controller.OnStatusUpdate += (sender, args) =>
            {
                if (InvokeRequired)
                {
                    Invoke(new StatusUpdateDelegate((newStatus) => StatusLabel.Text = newStatus), args.NewStatus);
                    return;
                }

                StatusLabel.Text = args.NewStatus;
            };

            _autoUpDown = new AutoUpDown(_controller, FlashWindow);
        }

        private delegate void FlashWindowDelegate();
        private void FlashWindow()
        {
            if (InvokeRequired)
            {
                Invoke(new FlashWindowDelegate(FlashWindow));
                return;
            }

            PsDeskController.FlashWindow.Flash(this, 5);
        }

        private void AllUpButton_Click(object sender, EventArgs e)
        {
            _controller.MoveUp();
        }

        private void StopButton_Click(object sender, EventArgs e)
        {
            _controller.StopImmediately();
        }

        private void AllDownButton_Click(object sender, EventArgs e)
        {
            _controller.MoveDown();
        }

        private void UpButton_MouseDown(object sender, MouseEventArgs e)
        {
            _controller.MoveUp();
        }

        private void UpButton_MouseUp(object sender, MouseEventArgs e)
        {
            _controller.StopImmediately();
        }

        private void DownButton_MouseDown(object sender, MouseEventArgs e)
        {
            _controller.MoveDown();
        }

        private void DownButton_MouseUp(object sender, MouseEventArgs e)
        {
            _controller.StopImmediately();
        }

        private void upDownEnabled_CheckedChanged(object sender, EventArgs e)
        {
            _autoUpDown.SetMode(upDownEnabled.Checked, (int)upTimeSpinner.Value, (int)downTimeSpinner.Value);
        }
    }
}
