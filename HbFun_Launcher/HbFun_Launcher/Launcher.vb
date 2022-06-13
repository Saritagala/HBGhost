Imports System.IO
Imports System.Net
Imports System.Reflection
Public Class Launcher
    Dim WC1 As WebClient
    Dim WC4 As WebClient
    Dim currentlauncher As String = Assembly.GetExecutingAssembly.GetName.Version.Major & "." & Assembly.GetExecutingAssembly.GetName.Version.Minor
    Dim newestversion As String
    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        WC1 = New WebClient
        WC4 = New WebClient
        If My.Computer.FileSystem.FileExists("Update.zip") Then
            File.Delete("Update.zip")
        End If
        If My.Computer.FileSystem.FileExists("Launcher.zip") Then
            File.Delete("Launcher.zip")
        End If
        Me.Text &= " (" & currentlauncher & ")"
    End Sub
    Private Sub DownloadLauncher()
        Try
            AddHandler WC4.DownloadProgressChanged, AddressOf ProgChanged4
            AddHandler WC4.DownloadFileCompleted, AddressOf DownComplete4
            WC4.DownloadFileAsync(New Uri("http://" & ip & "/Launcher.zip"), "Launcher.zip")
        Catch ex As Exception
            MsgBox(ex.Message, vbOKOnly + vbCritical, "Error")
            GenerateLog(ex.Message)
        End Try
    End Sub
    Private Sub ProgChanged4(sender As Object, e As DownloadProgressChangedEventArgs)
        Label1.Text = "Downloading Launcher... " & String.Format("{0}/{1} Mb", (e.BytesReceived / 1024D / 1024D).ToString("N2"), (e.TotalBytesToReceive / 1024D / 1024D).ToString("N2")) & " (" & e.ProgressPercentage.ToString("N0") & "%)"
        ProgressBar1.Value = e.ProgressPercentage
    End Sub
    Private Sub DownComplete4()
        WC4.Dispose()
        Label1.Text = ""
        Label3.Text = "Updating Launcher..."
        If MsgBox("Launcher need to be restarted to proceed with the update. Restart?", vbYesNo + vbInformation, "Launcher Update") = vbYes Then
            Try
                Me.Hide()
                RunProgram("Updater.exe")
                Me.Dispose()
            Catch ex As Exception
                MsgBox(ex.Message, vbOKOnly + vbCritical, "Error")
                GenerateLog(ex.Message)
            End Try
        Else
            Button1.Enabled = True
        End If
    End Sub
    Private Sub CheckVersion()
        Dim currentversion As String
        Dim request As HttpWebRequest
        Try
            currentversion = My.Computer.FileSystem.ReadAllText("version.txt")
            request = HttpWebRequest.Create("http://" & ip & "/version.txt")
            Using response As HttpWebResponse = request.GetResponse()
                Using sr As New StreamReader(response.GetResponseStream())
                    newestversion = sr.ReadToEnd()
                End Using
            End Using
            If newestversion = currentversion Then
                Label3.Text = "All files are up to date."
                Button1.Enabled = True
                ProgressBar1.Value = 100
            Else
                DownloadClient()
            End If
        Catch ex As Exception
            MsgBox(ex.Message, vbOKOnly + vbCritical, "Error")
            GenerateLog(ex.Message)
        End Try
    End Sub
    Private Sub DownloadClient()
        Try
            AddHandler WC1.DownloadProgressChanged, AddressOf ProgChanged
            AddHandler WC1.DownloadFileCompleted, AddressOf DownComplete
            WC1.DownloadFileAsync(New Uri("http://" & ip & "/Update.zip"), "Update.zip")
        Catch ex As Exception
            MsgBox(ex.Message, vbOKOnly + vbCritical, "Error")
            GenerateLog(ex.Message)
        End Try
    End Sub
    Private Sub ProgChanged(sender As Object, e As DownloadProgressChangedEventArgs)
        Label1.Text = "Downloading Update... " & String.Format("{0}/{1} Mb", (e.BytesReceived / 1024D / 1024D).ToString("N2"), (e.TotalBytesToReceive / 1024D / 1024D).ToString("N2")) & " (" & e.ProgressPercentage.ToString("N0") & "%)"
        ProgressBar1.Value = e.ProgressPercentage
    End Sub
    Private Sub DownComplete()
        WC1.Dispose()
        Label1.Text = ""
        Label3.Text = "Updating Files..."
        InstallClient()
        Label3.Text = "All files are up to date."
        Button1.Enabled = True
        ProgressBar1.Value = 100
        UpdateVersion(newestversion)
    End Sub
    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        Try
            Me.Hide()
            RegistryKeyFix()
            RunProgram("Game.exe")
            Me.Dispose()
        Catch ex As Exception
            MsgBox(ex.Message, vbOKOnly + vbCritical, "Error")
            GenerateLog(ex.Message)
        End Try
    End Sub
    Private Sub Launcher_FormClosing(sender As Object, e As FormClosingEventArgs) Handles MyBase.FormClosing
        ReleaseMemory()
    End Sub
    Private Sub Button2_Click(sender As Object, e As EventArgs) Handles Button2.Click
        Settings.Show()
    End Sub
    Private Sub Launcher_Shown(sender As Object, e As EventArgs) Handles MyBase.Shown
        Dim request As HttpWebRequest
        Dim newestlauncher As String
        Try
            Label3.Text = "Checking for updates..."
            wait(5)
            Label3.Text = ""
            request = HttpWebRequest.Create("http://" & ip & "/Launcher.txt")
            Using response As HttpWebResponse = request.GetResponse()
                Using sr As New StreamReader(response.GetResponseStream())
                    newestlauncher = sr.ReadToEnd()
                End Using
            End Using
            If newestlauncher <> currentlauncher Then
                DownloadLauncher()
            Else
                CheckVersion()
            End If
        Catch ex As Exception
            MsgBox(ex.Message, vbOKOnly + vbCritical, "Error")
            GenerateLog(ex.Message)
        End Try
    End Sub
End Class