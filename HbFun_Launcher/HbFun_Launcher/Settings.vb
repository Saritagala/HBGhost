Imports System.IO
Imports System.Text
Public Class Settings
    Dim actualText As String = ""
    Dim FILE_NAME As String = Application.StartupPath & "\GameConfig.cfg"
    Private Sub Settings_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        actualText = Me.Text
        LoadSettings()
        Me.Text = actualText
        Label3.Text = scrSound.Value
        Label4.Text = scrMusic.Value
    End Sub
    Private Sub HScrollBar1_Scroll(sender As Object, e As ScrollEventArgs) Handles scrSound.Scroll
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
        Label3.Text = scrSound.Value
    End Sub
    Private Sub HScrollBar2_Scroll(sender As Object, e As ScrollEventArgs) Handles scrMusic.Scroll
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
        Label4.Text = scrMusic.Value
    End Sub
    Private Sub Settings_FormClosing(sender As Object, e As FormClosingEventArgs) Handles MyBase.FormClosing
        If Me.Text.Contains("*") Then
            If MsgBox("Changes not saved, would you like to save?", vbYesNo + vbExclamation, "Save") = vbYes Then
                SaveSettings()
            End If
        End If
    End Sub
    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        SaveSettings()
    End Sub
    Private Sub SaveSettings()
        Dim FileLine(15) As String
        If Me.Text.Contains("*") Then
            Me.Text = actualText
            FileLine(0) = "[CONFIG]"
            FileLine(1) = "music-mode = " & chkMusic.CheckState
            FileLine(2) = "music-volume = " & scrMusic.Value
            FileLine(3) = "sound-mode = " & chkSound.CheckState
            FileLine(4) = "sound-volume = " & scrSound.Value
            FileLine(5) = "show-emblems = " & chkEmblems.CheckState
            FileLine(6) = "show-frames = " & chkFrames.CheckState
            FileLine(7) = "show-trees = " & chkTrees.CheckState
            FileLine(8) = "show-party-info = " & chkParty.CheckState
            FileLine(9) = "show-events = " & chkEvents.CheckState
            FileLine(10) = "detail-mode = " & cboDetail.SelectedIndex
            FileLine(11) = "guide-map = " & chkMap.CheckState
            FileLine(12) = "dialog-transparency = " & chkDialog.CheckState
            FileLine(13) = "shout-mode = " & chkShout.CheckState
            FileLine(14) = "whisper-mode = " & chkWhisper.CheckState
            FileLine(15) = "show-grid = " & chkGrid.CheckState
            Dim objWriter As New StreamWriter(FILE_NAME)
            For i = 0 To 15
                objWriter.WriteLine(FileLine(i))
            Next
            objWriter.Close()
        End If
        MsgBox("Changes saved!", vbOKOnly + vbInformation, "Done")
    End Sub
    Private Sub LoadSettings()
        Dim FileLine(15) As String
        Dim line As String()
        Dim reader As New StreamReader(FILE_NAME, Encoding.Default)
        For i = 0 To 15
            line = reader.ReadLine().Split("=")
            If line.Contains("[CONFIG]") Then Continue For
            FileLine(i) = line(1)
        Next
        reader.Close()
        chkMusic.CheckState = FileLine(1).Trim
        scrMusic.Value = FileLine(2).Trim
        chkSound.CheckState = FileLine(3).Trim
        scrSound.Value = FileLine(4).Trim
        chkEmblems.CheckState = FileLine(5).Trim
        chkFrames.CheckState = FileLine(6).Trim
        chkTrees.CheckState = FileLine(7).Trim
        chkParty.CheckState = FileLine(8).Trim
        chkEvents.CheckState = FileLine(9).Trim
        cboDetail.SelectedIndex = FileLine(10).Trim
        chkMap.CheckState = FileLine(11).Trim
        chkDialog.CheckState = FileLine(12).Trim
        chkShout.CheckState = FileLine(13).Trim
        chkWhisper.CheckState = FileLine(14).Trim
        chkGrid.CheckState = FileLine(15).Trim
    End Sub
    Private Sub chkMusic_CheckedChanged(sender As Object, e As EventArgs) Handles chkMusic.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub chkSound_CheckedChanged(sender As Object, e As EventArgs) Handles chkSound.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub chkEmblems_CheckedChanged(sender As Object, e As EventArgs) Handles chkEmblems.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub chkWhisper_CheckedChanged(sender As Object, e As EventArgs) Handles chkWhisper.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub chkGrid_CheckedChanged(sender As Object, e As EventArgs) Handles chkGrid.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub chkFrames_CheckedChanged(sender As Object, e As EventArgs) Handles chkFrames.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub chkShout_CheckedChanged(sender As Object, e As EventArgs) Handles chkShout.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub chkTrees_CheckedChanged(sender As Object, e As EventArgs) Handles chkTrees.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub chkParty_CheckedChanged(sender As Object, e As EventArgs) Handles chkParty.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub chkMap_CheckedChanged(sender As Object, e As EventArgs) Handles chkMap.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub chkDialog_CheckedChanged(sender As Object, e As EventArgs) Handles chkDialog.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub cboDetail_SelectedIndexChanged(sender As Object, e As EventArgs) Handles cboDetail.SelectedIndexChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub chkEvents_CheckedChanged(sender As Object, e As EventArgs) Handles chkEvents.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
End Class