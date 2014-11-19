namespace PsDeskController
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.AllUpButton = new System.Windows.Forms.Button();
            this.AllDownButton = new System.Windows.Forms.Button();
            this.DownButton = new System.Windows.Forms.Button();
            this.UpButton = new System.Windows.Forms.Button();
            this.StopButton = new System.Windows.Forms.Button();
            this.StatusLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // AllUpButton
            // 
            this.AllUpButton.Location = new System.Drawing.Point(12, 25);
            this.AllUpButton.Name = "AllUpButton";
            this.AllUpButton.Size = new System.Drawing.Size(118, 41);
            this.AllUpButton.TabIndex = 0;
            this.AllUpButton.Text = "All the way up";
            this.AllUpButton.UseVisualStyleBackColor = true;
            this.AllUpButton.Click += new System.EventHandler(this.AllUpButton_Click);
            // 
            // AllDownButton
            // 
            this.AllDownButton.Location = new System.Drawing.Point(12, 177);
            this.AllDownButton.Name = "AllDownButton";
            this.AllDownButton.Size = new System.Drawing.Size(118, 41);
            this.AllDownButton.TabIndex = 1;
            this.AllDownButton.Text = "All the way down";
            this.AllDownButton.UseVisualStyleBackColor = true;
            this.AllDownButton.Click += new System.EventHandler(this.AllDownButton_Click);
            // 
            // DownButton
            // 
            this.DownButton.Location = new System.Drawing.Point(12, 148);
            this.DownButton.Name = "DownButton";
            this.DownButton.Size = new System.Drawing.Size(118, 23);
            this.DownButton.TabIndex = 2;
            this.DownButton.Text = "Down";
            this.DownButton.UseVisualStyleBackColor = true;
            this.DownButton.MouseDown += new System.Windows.Forms.MouseEventHandler(this.DownButton_MouseDown);
            this.DownButton.MouseUp += new System.Windows.Forms.MouseEventHandler(this.DownButton_MouseUp);
            // 
            // UpButton
            // 
            this.UpButton.Location = new System.Drawing.Point(12, 72);
            this.UpButton.Name = "UpButton";
            this.UpButton.Size = new System.Drawing.Size(118, 23);
            this.UpButton.TabIndex = 3;
            this.UpButton.Text = "Up";
            this.UpButton.UseVisualStyleBackColor = true;
            this.UpButton.MouseDown += new System.Windows.Forms.MouseEventHandler(this.UpButton_MouseDown);
            this.UpButton.MouseUp += new System.Windows.Forms.MouseEventHandler(this.UpButton_MouseUp);
            // 
            // StopButton
            // 
            this.StopButton.Location = new System.Drawing.Point(12, 101);
            this.StopButton.Name = "StopButton";
            this.StopButton.Size = new System.Drawing.Size(118, 41);
            this.StopButton.TabIndex = 4;
            this.StopButton.Text = "STOP!";
            this.StopButton.UseVisualStyleBackColor = true;
            this.StopButton.Click += new System.EventHandler(this.StopButton_Click);
            // 
            // StatusLabel
            // 
            this.StatusLabel.AutoSize = true;
            this.StatusLabel.Location = new System.Drawing.Point(12, 9);
            this.StatusLabel.Name = "StatusLabel";
            this.StatusLabel.Size = new System.Drawing.Size(16, 13);
            this.StatusLabel.TabIndex = 5;
            this.StatusLabel.Text = "...";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(142, 230);
            this.Controls.Add(this.StatusLabel);
            this.Controls.Add(this.StopButton);
            this.Controls.Add(this.UpButton);
            this.Controls.Add(this.DownButton);
            this.Controls.Add(this.AllDownButton);
            this.Controls.Add(this.AllUpButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "MainForm";
            this.Text = "PsDesk";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button AllUpButton;
        private System.Windows.Forms.Button AllDownButton;
        private System.Windows.Forms.Button DownButton;
        private System.Windows.Forms.Button UpButton;
        private System.Windows.Forms.Button StopButton;
        private System.Windows.Forms.Label StatusLabel;
    }
}

