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
    }
}
