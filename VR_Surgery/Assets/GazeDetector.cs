using UnityEngine;
using System.Collections;

public class GazeDetector : MonoBehaviour
{
    public GameObject tool1;
    public GameObject tool2;

    private float gazeDuration = 5f;
    private float gazeTimer = 0f;
    private bool isGazing = false;
    private Coroutine gazeCoroutine;

    void Start()
    {
        // Ensure Tool1 is Active and Tool2 is Inactive at Start
        tool1.SetActive(true);
        tool2.SetActive(false);
    }

    void Update()
    {
        Ray ray = new Ray(transform.position, transform.forward);
        RaycastHit hit;

        if (Physics.Raycast(ray, out hit))
        {
            if (hit.collider.CompareTag("Rectangle"))
            {
                if (!isGazing)
                {
                    isGazing = true;
                    gazeCoroutine = StartCoroutine(StartGaze());
                }
            }
            else
            {
                ResetGaze();
            }
        }
        else
        {
            ResetGaze();
        }
    }

    IEnumerator StartGaze()
    {
        gazeTimer = 0f;
        while (gazeTimer < gazeDuration)
        {
            gazeTimer += Time.deltaTime;
            yield return null;
        }
        SwitchTools();
        ResetGaze();
    }

    void ResetGaze()
    {
        isGazing = false;
        if (gazeCoroutine != null)
        {
            StopCoroutine(gazeCoroutine);
            gazeCoroutine = null;
        }
        gazeTimer = 0f;
    }

    void SwitchTools()
    {
        tool1.SetActive(!tool1.activeSelf);
        tool2.SetActive(!tool2.activeSelf);
    }
}
