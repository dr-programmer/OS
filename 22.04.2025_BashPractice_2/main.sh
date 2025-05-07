src_dir="$1"
dest_dir="$2"
mode="$3"
shift 3

if [ ! -d "$src_dir" ]; then
  echo "Error: Source directory '$src_dir' does not exist."
  exit 1
fi

mkdir -p "$dest_dir"

if [ "$mode" = "type" ]; then
  for file in "$src_dir"/*; do
    if [ -f "$file" ]; then
      ext="${file##*.}"
      folder="${dest_dir}/${ext^^}"
      mkdir -p "$folder"
      cp "$file" "$folder/"
      echo "[Copied] $(basename "$file") -> ${ext^^}/"
    fi
  done

elif [ "$mode" = "keyword" ]; then
  if [ "$#" -lt 1 ]; then
    echo "Error: At least one keyword must be provided in 'keyword' mode."
    exit 1
  fi

  for file in "$src_dir"/*; do
    if [ -f "$file" ]; then
      filename=$(basename "$file")
      matched=false

      for keyword in "$@"; do
        echo "$filename" | grep -qi "$keyword"
        if [ $? -eq 0 ]; then
          folder="${dest_dir}/${keyword}"
          mkdir -p "$folder"
          cp "$file" "$folder/"
          echo "[Copied] $filename -> ${keyword}/"
          matched=true
          break
        fi
      done

      if [ "$matched" = false ]; then
        folder="${dest_dir}/others"
        mkdir -p "$folder"
        cp "$file" "$folder/"
        echo "[Copied] $filename -> others/"
      fi
    fi
  done

else
  echo "Error: Unknown mode '$mode'. Use 'type' or 'keyword'."
  exit 1
fi
