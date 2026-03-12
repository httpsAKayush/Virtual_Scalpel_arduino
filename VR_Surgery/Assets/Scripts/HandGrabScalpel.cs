using UnityEngine;

public class HandGrabScalpel : MonoBehaviour
{
    [Header("Grab Settings")]
    public Transform grabPoint; // where scalpel snaps to (optional)
    private GameObject grabbedObject;

    void OnTriggerEnter(Collider other)
    {
        if (grabbedObject != null)
            return;

        if (other.CompareTag("Scalpel"))
        {
            Grab(other.gameObject);
        }
    }

    void Grab(GameObject obj)
    {
        grabbedObject = obj;

        Rigidbody rb = obj.GetComponent<Rigidbody>();
        if (rb != null)
        {
            rb.isKinematic = true;
            rb.useGravity = false;
        }

        obj.transform.SetParent(grabPoint != null ? grabPoint : transform);
        obj.transform.localPosition = Vector3.zero;

        // Match grab point orientation (NOT identity)
        obj.transform.localRotation = Quaternion.identity;


        Debug.Log("[Grab] Scalpel grabbed");
    }

    // 🔓 Call this later (sensor / button / logic)
    public void Release()
    {
        if (grabbedObject == null)
            return;

        Rigidbody rb = grabbedObject.GetComponent<Rigidbody>();
        if (rb != null)
        {
            rb.isKinematic = false;
            rb.useGravity = true;
        }

        grabbedObject.transform.SetParent(null);
        grabbedObject = null;

        Debug.Log("[Grab] Scalpel released");
    }
}