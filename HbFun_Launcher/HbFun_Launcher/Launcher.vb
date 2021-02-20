Imports System.IO
Imports System.Net
Imports System.Reflection
Imports SharpCompress.Archive
Imports SharpCompress.Common
Public Class Launcher
    Dim WC1 As WebClient
    Dim WC3 As WebClient
    Dim WC4 As WebClient
    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        WC1 = New WebClient
        WC3 = New WebClient
        WC4 = New WebClient
        If My.Computer.FileSystem.FileExists("Update.zip") Then
            File.Delete("Update.zip")
        End If
        If My.Computer.FileSystem.FileExists("Launcher.zip") Then
            File.Delete("Launcher.zip")
        End If
        Try
            Dim currentversion As String = Assembly.GetExecutingAssembly.GetName.Version.Major & "." & Assembly.GetExecutingAssembly.GetName.Version.Minor
            Dim request As HttpWebRequest = HttpWebRequest.Create("http://162.248.93.248/Launcher.txt")
            Dim response As HttpWebResponse = request.GetResponse()
            Dim sr As New StreamReader(response.GetResponseStream())
            Dim newestversion As String = sr.ReadToEnd()
            Me.Text &= " (" & currentversion & ")"
            If newestversion <> currentversion Then
                DownloadLauncher()
            Else
                CheckVersion()
            End If
        Catch ex As Exception
            MsgBox(ex.Message)
        End Try
    End Sub
    Private Sub DownloadLauncher()
        Try
            AddHandler WC4.DownloadProgressChanged, AddressOf ProgChanged4
            AddHandler WC4.DownloadFileCompleted, AddressOf DownComplete4
            WC4.DownloadFileAsync(New Uri("http://162.248.93.248/Launcher.zip"), "Launcher.zip")
        Catch ex As Exception
            MsgBox(ex.Message)
        End Try
    End Sub
    Private Sub ProgChanged4(sender As Object, e As DownloadProgressChangedEventArgs)
        Label1.Text = "Downloading Launcher... " & String.Format("{0}/{1} Mb", (e.BytesReceived / 1024D / 1024D).ToString("N2"), (e.TotalBytesToReceive / 1024D / 1024D).ToString("N2")) & " (" & e.ProgressPercentage.ToString("N0") & "%)"
        ProgressBar1.Value = e.ProgressPercentage
    End Sub
    Private Sub DownComplete4()
        WC4.Dispose()
        Try
            Dim appPath As String = AppDomain.CurrentDomain.BaseDirectory()
            Dim archive As IArchive = ArchiveFactory.Open("Launcher.zip")
            For Each entry In archive.Entries
                If Not entry.IsDirectory Then
                    entry.WriteToDirectory(appPath & "Launcher", ExtractOptions.ExtractFullPath Or ExtractOptions.Overwrite)
                End If
            Next
        Catch ex As Exception
        End Try
        Label1.Text = ""
        If MsgBox("Launcher need to be restarted to proceed with the update. Restart?", vbYesNo + vbExclamation, "Launcher Update") = vbYes Then
            Me.Hide()
            Process.Start("Updater.exe")
            Me.Dispose()
        Else
            Button1.Enabled = True
            Button1.Text = "Restart"
        End If
    End Sub
    Private Sub CheckVersion()
        Try
            Dim currentversion As String = My.Computer.FileSystem.ReadAllText("version.txt")
            Dim request As HttpWebRequest = HttpWebRequest.Create("http://162.248.93.248/version.txt")
            Dim response As HttpWebResponse = request.GetResponse()
            Dim sr As New StreamReader(response.GetResponseStream())
            Dim newestversion As String = sr.ReadToEnd()
            If newestversion = currentversion Then
                Label3.Text = "All files are up to date."
                Button1.Enabled = True
                ProgressBar1.Value = 100
            Else
                DownloadClient()
            End If
        Catch ex As Exception
            MsgBox(ex.Message)
        End Try
    End Sub
    Private Sub DownloadVersion()
        File.Delete("version.txt")
        Try
            AddHandler WC3.DownloadFileCompleted, AddressOf DownComplete3
            WC3.DownloadFileAsync(New Uri("http://162.248.93.248/version.txt"), "version.txt")
        Catch ex As Exception
            MsgBox(ex.Message)
        End Try
    End Sub
    Private Sub DownComplete3()
        WC3.Dispose()
        InstallClient()
        Label1.Text = ""
        Label3.Text = "All files are up to date."
        Button1.Enabled = True
        ProgressBar1.Value = 100
    End Sub
    Private Sub DownloadClient()
        Try
            AddHandler WC1.DownloadProgressChanged, AddressOf ProgChanged
            AddHandler WC1.DownloadFileCompleted, AddressOf DownComplete
            WC1.DownloadFileAsync(New Uri("http://162.248.93.248/Update.zip"), "Update.zip")
        Catch ex As Exception
            MsgBox(ex.Message)
        End Try
    End Sub
    Private Sub ProgChanged(sender As Object, e As DownloadProgressChangedEventArgs)
        Label1.Text = "Downloading Update... " & String.Format("{0}/{1} Mb", (e.BytesReceived / 1024D / 1024D).ToString("N2"), (e.TotalBytesToReceive / 1024D / 1024D).ToString("N2")) & " (" & e.ProgressPercentage.ToString("N0") & "%)"
        ProgressBar1.Value = e.ProgressPercentage
    End Sub
    Private Sub DownComplete()
        WC1.Dispose()
        DownloadVersion()
    End Sub
    Private Sub InstallClient()
        File.Delete("Game.exe")
        Try
            Dim appPath As String = AppDomain.CurrentDomain.BaseDirectory()
            Dim archive As IArchive = ArchiveFactory.Open("Update.zip")
            For Each entry In archive.Entries
                If Not entry.IsDirectory Then
                    entry.WriteToDirectory(appPath, ExtractOptions.ExtractFullPath Or ExtractOptions.Overwrite)
                End If
            Next
        Catch ex As Exception
            MsgBox(ex.Message)
        End Try
    End Sub
    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        Me.Hide()
        RegistryKeyFix()
        If Button1.Text = "Play" Then
            Process.Start("Game.exe")
        Else
            Process.Start("Updater.exe")
        End If
        Me.Dispose()
    End Sub
    Private Sub Launcher_FormClosing(sender As Object, e As FormClosingEventArgs) Handles MyBase.FormClosing
        Me.Dispose()
    End Sub
    Private Sub Button2_Click(sender As Object, e As EventArgs) Handles Button2.Click
        Settings.ShowDialog()
    End Sub
    Private Sub RegistryKeyFix()
        Dim osversion As Version = Environment.OSVersion.Version
        If (osversion.Major = 6 And osversion.Minor > 1) Or (osversion.Major = 10 And osversion.Minor = 0) Then
            My.Computer.Registry.SetValue("HKEY_CURRENT_USER\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers", Application.StartupPath & "\Game.exe", "Layer_ForceDirectDrawEmulation")
        End If
    End Sub
End Class