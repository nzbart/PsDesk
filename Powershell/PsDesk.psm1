$ErrorActionPreference = 'Stop'
$port = new-object System.IO.Ports.SerialPort
    
function Expect($port, $what) {
    $r = $port.ReadLine().Replace("`r", "")
    if ($r -ine $what) {
        $port.Write('!')
        Disconnect-Desk
        throw "Expected $what, received $r. Disconnect."
    }
}

function Connect-Desk() {
    $port.BaudRate = 19200
    $port.PortName = "COM7"
    $port.DataBits = 8;
    $port.Parity = [System.IO.Ports.Parity]::None;
    $port.StopBits = 1;
    $port.Open()

    1..3 | % {
        $port.WriteLine($_)
        sleep 0.1
        $port.ReadLine()
    }
    sleep 1
    if ($port.BytesToRead -gt 0) {
        $port.ReadLine()
    }
}

function Raise-Desk() {
    $port.WriteLine('U')
    Expect $port "OK U"
    Expect $port "START"
    WaitForFinishWithAbortSupport $port
}

function Lower-Desk() {
    $port.WriteLine('D')
    Expect $port "OK D"
    Expect $port "START"
    WaitForFinishWithAbortSupport $port
}

function WaitForFinishWithAbortSupport($port)
{
    while ($port.BytesToRead -lt 1) {
        if ($Host.UI.RawUI.KeyAvailable) {
            $port.Write('!');
            Disconnect-Desk
        }

        sleep 0.1
    }

    Expect $port "DONE"
}

function Disconnect-Desk() {
    $port.Close()
}

Export-ModuleMember Connect-Desk
Export-ModuleMember Disconnect-Desk
Export-ModuleMember Raise-Desk
Export-ModuleMember Lower-Desk
