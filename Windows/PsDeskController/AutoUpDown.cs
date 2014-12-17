using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms.VisualStyles;

namespace PsDeskController
{
    public class AutoUpDown
    {
        readonly Timer _timer;

        private bool _up = true;
        private bool _enabled = false;
        private int _upMinutes = 0;
        private int _downMinutes = 0;
        private readonly DeskController _controller;
        private readonly Action _preChangeWarning;

        public AutoUpDown(DeskController controller, Action preChangeWarning)
        {
            _timer = new Timer(TimerTick, this, TimeSpan.FromMinutes(1), TimeSpan.FromMinutes(1));
            _controller = controller;
            _preChangeWarning = preChangeWarning;
        }

        public void SetMode(bool enabled, int upMinutes, int downMinutes)
        {
            _enabled = enabled;
            _upMinutes = upMinutes;
            _downMinutes = downMinutes;

            _up = true;
            ThreadPool.QueueUserWorkItem(o => ToggleDesk());
        }

        private void ToggleDesk()
        {
            if (!_enabled)
            {
                return;
            }

            if (_preChangeWarning != null)
            {
                _preChangeWarning();
                Thread.Sleep(TimeSpan.FromSeconds(5));
            }

            if (_up)
            {
                _controller.MoveUp();
                _timer.Change(TimeSpan.FromMinutes(_upMinutes), TimeSpan.FromMinutes(_upMinutes));
            }
            else
            {
                _controller.MoveDown();
                _timer.Change(TimeSpan.FromMinutes(_downMinutes), TimeSpan.FromMinutes(_downMinutes));
            }

            _up = !_up;
        }

        private static void TimerTick(object state)
        {
            ((AutoUpDown)state).ToggleDesk();
        }
    }
}
