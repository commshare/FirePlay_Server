using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class MatchUIManager : MonoBehaviour
{
    private GameObject _curtain = null;
    private GUIStyle _textStyle = null;
    private bool _isTryingNumberMatch = false;
    private bool _isTryingFastMatch = false;

    private void Awake()
    {
        _textStyle = new GUIStyle();
        _textStyle.fontSize = 40;
        _textStyle.normal.textColor = Color.white;
        _curtain = Instantiate(Resources.Load("Prefabs/BlackCurtain") as GameObject);
        MakeAnim();
    }

    private void OnGUI()
    {
        MakeButton();
    }

    private void MakeAnim()
    {
        var selectInfo = FindObjectOfType<PlayerInfo>();

        if (selectInfo == null) return;

        string prefabPath = "PrivateData/SpritesArchers/FantasyArcher_0" + (int)selectInfo._selectedPlayerType;

        var instance = Instantiate(Resources.Load(prefabPath) as GameObject);

        instance.GetComponent<Transform>().position = new Vector3(0f, 1f, 0f);
    }

    private void MakeButton()
    {
        if (_isTryingFastMatch == false && _isTryingNumberMatch == false)
        {
            if (GUI.Button(new Rect((Screen.width / 2) - 155, (Screen.height * 2 / 3), 150, 150), "Fast Match"))
            {
                TryFastMatch();
            }

            if (GUI.Button(new Rect((Screen.width / 2) + 5, (Screen.height * 2 / 3), 150, 150), "Number Match"))
            {
                TryNumberMatch();
                // TODO :: Number Match는 추후에 구현.
            }
        }
        else if (_isTryingNumberMatch == true)
        {
            GUI.Label(new Rect((Screen.width / 2) - 100, Screen.height / 3, 200, 100), "Matching...", _textStyle);

            if (GUI.Button(new Rect((Screen.width / 2) - 100, Screen.height * 2 / 3, 200, 100), "Cancel"))
            {
                _curtain.GetComponent<BlackCurtain>().StartFadeOut();
                _isTryingNumberMatch = false;
            }
        }
        else
        {

        }
    }

    private void TryFastMatch()
    {
        SceneManager.LoadScene("Game");
    }

    private void TryNumberMatch()
    {
        _curtain.GetComponent<BlackCurtain>().StartFadeIn();

        _isTryingNumberMatch = true;
    }

}
