Imports SharpCompress.Archive
Imports SharpCompress.Common
Module Update
    Sub Main()
        Try
            Dim appPath As String = AppDomain.CurrentDomain.BaseDirectory()
            Dim archive As IArchive = ArchiveFactory.Open("Launcher.zip")
            For Each entry In archive.Entries
                If Not entry.IsDirectory Then
                    entry.WriteToDirectory(appPath, ExtractOptions.ExtractFullPath Or ExtractOptions.Overwrite)
                End If
            Next
        Catch ex As Exception
        End Try
        Process.Start("Launcher.exe")
    End Sub
End Module