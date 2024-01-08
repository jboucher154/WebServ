const radioButtons = document.querySelectorAll('.Pet_Radio input[type="radio"]');
	const labels = document.querySelectorAll('.Pet_Radio label');

	// Add click event listener to each radio button
	radioButtons.forEach((radioButton, index) => {
		radioButton.addEventListener('click', () => {
			// Reset color for all labels
			labels.forEach(label => {
				label.style.backgroundColor = '';
				label.style.color = '';
			});

			// Set color for the selected label
			labels[index].style.backgroundColor = '#3CB371';
			labels[index].style.color = 'white';
		});
	});

// document.getElementById('mainForm').addEventListener('submit', function(event) {
	// 	event.preventDefault(); // Prevent the default form submission

	// 	// Submit the main form
	// 	const mainForm = document.getElementById('mainForm');
	// 	const formData = new FormData(mainForm);
	// 	fetch(mainForm.action, {
	// 		method: 'POST',
	// 		body: formData
	// 	});

	// 	// Submit the photo form
	// 	const photoForm = document.getElementById('photoForm');
	// 	const photoFormData = new FormData(photoForm);
	// 	fetch(photoForm.action, {
	// 		method: 'POST',
	// 		body: photoFormData
	// 	});
	// });