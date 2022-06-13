Imports System.IO
Imports SharpCompress.Archive
Imports SharpCompress.Common
Module Update
    Sub Main()
        Try
            Dim archive As IArchive = ArchiveFactory.Open("Launcher.zip")
            For Each entry In archive.Entries
                If Not entry.IsDirectory Then
                    entry.WriteToDirectory(AppDomain.CurrentDomain.BaseDirectory(), ExtractOptions.ExtractFullPath Or ExtractOptions.Overwrite)
                End If
            Next
            RunProgram("Launcher.exe")
        Catch ex As Exception
            GenerateLog(ex.Message)
        End Try
    End Sub
    Sub GenerateLog(msg As String)
        Using sw As New StreamWriter("ErrorLog_" & DateTime.Now.ToString("yyyyMMdd") & ".log", True)
            sw.WriteLine(Now.ToString("HH:mm:ss") & " (Updater) : " & msg)
        End Using
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