document.getElementById('navbar_button').addEventListener("click", function(event) {
    const el = document.getElementById('mobile_navbar');
    if (el.classList.contains('hidden')) {
        el.classList.remove('hidden');
    } else {
        el.classList.add('hidden');
    }
});
