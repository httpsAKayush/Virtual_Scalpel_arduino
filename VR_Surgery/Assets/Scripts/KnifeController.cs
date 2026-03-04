// using UnityEngine;

// public class KnifeController : MonoBehaviour
// {
//     private Vector3 previousPosition;
//     public LayerMask sliceMask;

//     private void Start()
//     {
//         previousPosition = transform.position;
//     }

//     void Update()
//     {
//         Vector3 currentPosition = transform.position;
//         Vector3 movementDirection = currentPosition - previousPosition;

//         if (movementDirection.magnitude > 0.01f)
//         {
//             RaycastHit hit;
//             if (Physics.Raycast(transform.position, transform.forward, out hit, 5f, sliceMask))
//             {
//                 Sliceable sliceable = hit.collider.GetComponent<Sliceable>();
//                 if (sliceable != null)
//                 {
//                     sliceable.Slice(hit.point, transform.up); // Use knife's "up" as cut plane
//                 }
//             }
//         }

//         previousPosition = currentPosition;
//     }
// }
