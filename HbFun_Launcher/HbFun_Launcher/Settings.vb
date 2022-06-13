Imports System.IO
Public Class Settings
    Dim actualText As String
    Dim fileName As String = AppDomain.CurrentDomain.BaseDirectory() & "CONTENTS\GameConfig.cfg"
    Dim fileLength As Integer = 20
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
        Me.Close()
    End Sub
    Private Sub SaveSettings()
        Dim FileLine(fileLength) As String
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
            FileLine(16) = "show-npc = " & chkShowNPC.CheckState
            FileLine(17) = "quest-helper = " & chkQuestHelper.CheckState
            FileLine(18) = "show-big-items = " & chkBigItems.CheckState
            FileLine(19) = "use-old-panels = " & chkOldPanels.CheckState
            FileLine(20) = "auto-ek-ss = " & chkAutoSS.CheckState
            Try
                Using objWriter As New StreamWriter(fileName)
                    For i = 0 To fileLength
                        objWriter.WriteLine(FileLine(i))
                    Next
                End Using
            Catch ex As Exception
                GenerateLog(ex.Message)
                Exit Sub
            End Try
        End If
        MsgBox("Changes saved!", vbOKOnly + vbInformation, "Done")
    End Sub
    Private Sub LoadSettings()
        Dim FileLine(fileLength) As String
        Dim line As String
        Dim fieldValue As String()
        Try
            Using reader As New StreamReader(fileName)
                For i = 0 To fileLength
                    line = reader.ReadLine()
                    If line.Contains("[CONFIG]") Then Continue For
                    fieldValue = line.Split("=")
                    FileLine(i) = fieldValue(1).Trim
                Next
            End Using
        Catch ex As Exception
            GenerateLog(ex.Message)
            Exit Sub
        End Try
        chkMusic.CheckState = FileLine(1)
        scrMusic.Value = FileLine(2)
        chkSound.CheckState = FileLine(3)
        scrSound.Value = FileLine(4)
        chkEmblems.CheckState = FileLine(5)
        chkFrames.CheckState = FileLine(6)
        chkTrees.CheckState = FileLine(7)
        chkParty.CheckState = FileLine(8)
        chkEvents.CheckState = FileLine(9)
        cboDetail.SelectedIndex = FileLine(10)
        chkMap.CheckState = FileLine(11)
        chkDialog.CheckState = FileLine(12)
        chkShout.CheckState = FileLine(13)
        chkWhisper.CheckState = FileLine(14)
        chkGrid.CheckState = FileLine(15)
        chkShowNPC.CheckState = FileLine(16)
        chkQuestHelper.CheckState = FileLine(17)
        chkBigItems.CheckState = FileLine(18)
        chkOldPanels.CheckState = FileLine(19)
        chkAutoSS.CheckState = FileLine(20)
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
    Private Sub chkShowNPC_CheckedChanged(sender As Object, e As EventArgs) Handles chkShowNPC.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub CheckBox1_CheckedChanged(sender As Object, e As EventArgs) Handles chkQuestHelper.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub chkBigItems_CheckedChanged(sender As Object, e As EventArgs) Handles chkBigItems.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub CheckBox1_CheckedChanged_1(sender As Object, e As EventArgs) Handles chkOldPanels.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
    Private Sub CheckBox1_CheckedChanged_2(sender As Object, e As EventArgs) Handles chkAutoSS.CheckedChanged
        If Not Me.Text.Contains("*") Then Me.Text &= "*"
    End Sub
End Class