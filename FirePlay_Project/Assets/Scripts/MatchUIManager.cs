using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MatchUIManager : MonoBehaviour
{
    private void OnGUI()
    {
        MakeButton();
    }

    private void MakeButton()
    {
        if (GUI.Button(new Rect((Screen.width / 2) - 155, (Screen.height * 2 / 3), 150, 150), "Fast Match"))
        {
            TryFastMatch();
        }

        if (GUI.Button(new Rect((Screen.width / 2) + 5, (Screen.height * 2 / 3), 150, 150), "Number Match"))
        {
            // TODO :: Number Match는 추후에 구현.
        }
    }

    private void TryFastMatch()
    {

    }
}
