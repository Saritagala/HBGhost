Imports System.IO
Imports SharpCompress.Archive
Imports SharpCompress.Common
Imports VB = Microsoft.VisualBasic
Module Module1
    Private Declare Auto Function SetProcessWorkingSetSize Lib "kernel32.dll" (procHandle As IntPtr, min As Int32, max As Int32) As Boolean
    'Public ip As String = "181.44.105.64:8080"
    Public ip As String = "192.168.0.28:8080"
    Public Sub ReleaseMemory()
        Try
            SetProcessWorkingSetSize(Process.GetCurrentProcess().Handle, -1, -1)
        Catch ex As Exception
            GenerateLog(ex.Message)
        End Try
    End Sub
    Public Sub RegistryKeyFix()
        Try
            Dim osversion As Version = Environment.OSVersion.Version
            If (osversion.Major = 6 And osversion.Minor > 1) Or (osversion.Major = 10 And osversion.Minor = 0) Then
                My.Computer.Registry.SetValue("HKEY_CURRENT_USER\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers", Application.StartupPath & "\Game.exe", "Layer_ForceDirectDrawEmulation DWM8And16BitMitigation 8And16BitAggregateBlts") ' 
            End If
        Catch ex As Exception
            GenerateLog(ex.Message)
        End Try
    End Sub
    Public Sub wait(seconds As Single)
        Static start As Single = VB.Timer()
        Do While VB.Timer() < start + seconds
            Application.DoEvents()
        Loop
    End Sub
    Public Sub GenerateLog(msg As String)
        Using sw As New StreamWriter("ErrorLog_" & DateTime.Now.ToString("yyyyMMdd") & ".log", True)
            sw.WriteLine(Now.ToString("HH:mm:ss") & " (Launcher) : " & msg)
        End Using
    End Sub
    Public Sub UpdateVersion(ver As String)
        Using sw As New StreamWriter("version.txt")
            sw.WriteLine(ver)
        End Using
    End Sub
    Public Sub InstallClient()
        Try
            Dim archive As IArchive = ArchiveFactory.Open("Update.zip")
            For Each entry In archive.Entries
                If Not entry.IsDirectory Then
                    entry.WriteToDirectory(AppDomain.CurrentDomain.BaseDirectory(), ExtractOptions.ExtractFullPath Or ExtractOptions.Overwrite)
                End If
            Next
        Catch ex As Exception
            GenerateLog(ex.Message)
        End Try
    End Sub
    Public Sub RunProgram(prg As String)
        Dim psi As New ProcessStartInfo
        With psi
            .FileName = AppDomain.CurrentDomain.BaseDirectory() & prg
            .Verb = "runas"
        End With
        Process.Start(psi)
    End Sub
End Module