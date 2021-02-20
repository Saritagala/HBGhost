Imports System.IO
Module Update
    Sub Main()
        File.Delete("Launcher.exe")
        File.Copy(Application.StartupPath & "\Launcher\Launcher.exe", "Launcher.exe")
        Directory.Delete(Application.StartupPath & "\Launcher", True)
        Process.Start("Launcher.exe")
    End Sub
End Module