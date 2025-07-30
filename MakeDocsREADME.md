
To test docs:
1) Run MakeDocs.sh
2) <your-browser-command> docs/html/index.html

To deploy docs:

1) switch to a branch named docs, and merge in master if needed.
2) Make sure public/ is not in .gitignore.
3) run MakeDocs.sh
4) commit
5) push docs to gitlab
6) wait a few seconds or a minute.
7) Navigate to douglas_s_leonard.gitlab.io/physics_utilities/VolumeBuilders

