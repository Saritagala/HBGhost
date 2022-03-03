Module Module1
    Private Declare Auto Function SetProcessWorkingSetSize Lib "kernel32.dll" (procHandle As IntPtr, min As Int32, max As Int32) As Boolean
    'Public ip As String = "181.44.105.64:8080"
    Public ip As String = "192.168.0.28:8080"
    Public Sub LiberarMemoria()
        Try
            SetProcessWorkingSetSize(Process.GetCurrentProcess().Handle, -1, -1)
        Catch ex As Exception
            MsgBox(ex.Message)
        End Try
    End Sub
    Public Sub RegistryKeyFix()
        Dim osversion As Version = Environment.OSVersion.Version
        If (osversion.Major = 6 And osversion.Minor > 1) Or (osversion.Major = 10 And osversion.Minor = 0) Then
            My.Computer.Registry.SetValue("HKEY_CURRENT_USER\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers", Application.StartupPath & "\Game.exe", "Layer_ForceDirectDrawEmulation DWM8And16BitMitigation 8And16BitAggregateBlts") ' 
        End If
    End Sub
End Module